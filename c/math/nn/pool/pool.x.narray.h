#ifndef LAUD_POOL_X_NARRAY_H
#define LAUD_POOL_X_NARRAY_H

#include <Ubject.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "BaseObject.h"
#include "mem_lk.h"
#include "pool.r.h"

#ifndef LAUD_NARRAY_IMPLEMENTATION
#error this must be #included in narray.c only
#define LAUD_NARRAY_IMPLEMENTATION
#endif

#include "../../../core/narray.h"
#include "../../../core/narray.r.static.h"
#include "../../../core/narray_bc.r.static.h"
#include "../../../math/nn/conv_pool_static_fn.h"

static void *narray_pool(const struct laud_narray *input_image,
                         const void *pool_class, const uint64_t *pool_size,
                         uint64_t *stride, uint64_t *padding) {
  const uint64_t image_rank = rank(input_image);
  const uint64_t *image_shape = shape(input_image);
  number_t *image_values = values(input_image);

  uint64_t image_start_index[image_rank];
  uint64_t image_index[image_rank];
  uint64_t image_multiplier[image_rank];
  compute_multiplier(image_multiplier, image_shape, image_rank);

  // Setup for pooling filters
  const uint64_t filter_rank = image_rank;
  const uint64_t filter_length =
      compute_length_from_shape(pool_size, filter_rank);
  uint64_t filter_index[filter_rank];
  uint64_t filter_multiplier[filter_rank];
  compute_multiplier(filter_multiplier, pool_size, filter_rank);

  laud_pool_compute_fn_t pool_algo = laud_pool_compute_fn(pool_class);
  uint64_t chosen_indexes[filter_length];

  // Handle padding and stride initialization
  uint64_t active_paddings[4 * image_rank];
  memset(active_paddings, 0, sizeof(active_paddings));
  initialize_padding_stride(&stride, padding, active_paddings, image_rank);

  // Prepare output array
  uint64_t output_shape[image_rank];
  uint64_t output_index[filter_rank];
  memset(output_index, 0, sizeof(output_index));
  compute_conv_pool_output_shape(image_shape, pool_size, output_shape,
                                 image_rank, filter_rank, stride,
                                 active_paddings);

  struct laud_narray *output = laud_narray(filter_rank, output_shape, 0, NULL);
  const uint64_t output_size = length(output);
  number_t *output_values = values(output);

  // Allocate meta data and padding information
  uint64_t *meta =
      MALLOC(4 * image_rank * sizeof(uint64_t) +
             laud_pool_meta_size(pool_class, image_rank, image_shape,
                                 image_rank, output_shape));

  if (!meta) {
    // Handle allocation error
    return NULL;
  }

  memcpy(meta, active_paddings, sizeof(active_paddings));
  void *meta_holder = (void *)(meta + 4 * image_rank);
  set_meta_data(output, meta);

  // Main pooling loop
  for (uint64_t output_flat_index = 0; output_flat_index < output_size;
       output_flat_index++) {
    output_values[output_flat_index] = 0;

    // Map output index to the starting index in the input image
    for (uint64_t i = 0; i < image_rank; i++) {
      image_index[i] = image_start_index[i] =
          (int64_t)output_index[i] * (int64_t)stride[i] -
          (int64_t)active_paddings[i * 2];
    }

    memset(filter_index, 0, filter_rank * sizeof(*filter_index));

    // Iterate through filter elements
    for (uint64_t iteration = 0; iteration < filter_length; iteration++) {
      if (!should_skip_conv_pool_operation(image_index, image_shape,
                                           image_rank)) {
        chosen_indexes[iteration] =
            compute_flat_index(image_index, image_multiplier, image_rank);
      }

      // Update image and filter indices for next iteration
      update_image_and_filter_indices(image_start_index, image_index,
                                      filter_index, pool_size, image_rank);
    }

    // Apply pooling algorithm
    pool_algo(image_values, filter_length, chosen_indexes,
              &output_values[output_flat_index], output_flat_index,
              meta_holder);

    // Update the output index
    update_index(output_index, output_shape, NULL, NULL, filter_rank);
  }

  return output;
}

char check_index_in_pool_window(const uint64_t *current_pool_start_idx,
                                const uint64_t *derivative_idx,
                                uint64_t input_rank,
                                const uint64_t *kernel_shape) {
  for (uint64_t i = 0; i < input_rank; i++) {
    if (derivative_idx[i] < current_pool_start_idx[i] ||
        derivative_idx[i] >= (current_pool_start_idx[i] + kernel_shape[i])) {
      return 0; // Derivative index is out of bounds
    }
  }
  return 1; // Derivative index is within the pooling window
}

void compute_pool_indexes(uint64_t *pool_idx,
                          const uint64_t *current_pool_start_idx,
                          uint64_t *pool_indexes,
                          const uint64_t *input_multiplier, uint64_t input_rank,
                          uint64_t kernel_size) {

  // Initialize pool index with the adjusted index
  memcpy(pool_idx, current_pool_start_idx, input_rank * sizeof(uint64_t));

  // Loop through all elements in the pool window
  for (uint64_t j = 0; j < kernel_size; j++) {
    // Compute flat index for the current pool element
    pool_indexes[j] =
        compute_flat_index(pool_idx, input_multiplier, input_rank);

    // Update pool_idx for the next element
    uint64_t pool_dim = input_rank - 1;
    pool_idx[pool_dim]++;

    // Handle overflow of pool index
    while (pool_idx[pool_dim] >=
           current_pool_start_idx[pool_dim] + input_multiplier[pool_dim]) {
      pool_idx[pool_dim] = current_pool_start_idx[pool_dim];

      if (pool_dim == 0)
        break; // If we have iterated through all dimensions
      pool_dim--;
      pool_idx[pool_dim]++;
    }
  }
}

void *laud_narray_dpool(const struct laud_narray *input_image,
                        const uint64_t respect_flag,
                        const struct laud_narray *precomputed_derivatives,
                        const void *pool_class,
                        const struct laud_narray *pooled_image) {

  // Fetch input image properties
  const uint64_t input_rank = rank(input_image);
  const uint64_t *input_shape = shape(input_image);
  const uint64_t input_size = length(input_image);
  const number_t *input_values = values(input_image);

  // Initialize index and multiplier arrays for input image
  uint64_t input_idx[input_rank];
  uint64_t input_multiplier[input_rank];
  compute_multiplier(input_multiplier, input_shape, input_rank);

  // Get pooling meta-data (padding, stride, filter shape)
  const uint64_t *padding_info = get_meta_data(pooled_image);
  const uint64_t *stride = padding_info + 2 * input_rank;
  const uint64_t *filter_shape = stride + input_rank;
  const void *meta = (void *)(filter_shape + input_rank);

  // Fetch kernel properties
  const uint64_t kernel_rank = input_rank;
  const uint64_t kernel_size =
      compute_length_from_shape(filter_shape, kernel_rank);

  // Initialize kernel index and multiplier arrays
  uint64_t kernel_multiplier[kernel_rank];
  compute_multiplier(kernel_multiplier, filter_shape, kernel_rank);

  // Fetch pooled image properties
  const uint64_t *pooled_shape = shape(pooled_image);
  const uint64_t pooled_size = length(pooled_image);

  // Initialize pooling indices and multipliers
  uint64_t pooling_idx[input_rank];
  memset(pooling_idx, 0, sizeof(pooling_idx));
  uint64_t pooling_multiplier[input_rank];
  compute_multiplier(pooling_multiplier, pooled_shape, input_rank);

  // Prepare to compute derivatives
  struct laud_narray *derivatives = NULL;
  const number_t *precomputed_derivatives_values =
      precomputed_derivatives ? values(precomputed_derivatives) : NULL;

  laud_pool_differentiate_fn_t dpool_algo =
      laud_pool_differentiate_fn(pool_class);

  if (respect_flag == 0) {
    // Initialize derivative output array
    derivatives = laud_narray(input_rank, input_shape, 0, NULL);
    number_t *derivative_values = values(derivatives);
    uint64_t derivative_idx[input_rank];
    memset(derivative_idx, 0, sizeof(derivative_idx));

    // Arrays to store indices for pooling computations
    uint64_t pool_indexes[kernel_size];
    uint64_t pool_idx[input_rank];

    // Main loop over all derivative elements
    for (uint64_t derivative_flat_idx = 0; derivative_flat_idx < input_size;
         derivative_flat_idx++) {

      // Reset adjusted indices
      uint64_t current_pool_start_idx[input_rank];
      memset(current_pool_start_idx, 0, sizeof(current_pool_start_idx));

      // Loop over all pooled elements
      for (uint64_t pooled_flat_idx = 0; pooled_flat_idx < pooled_size;
           pooled_flat_idx++) {

        // Check if current derivative index is within the pool window
        if (check_index_in_pool_window(current_pool_start_idx, derivative_idx,
                                       input_rank, filter_shape)) {

          // Compute pool indexes
          compute_pool_indexes(pool_idx, current_pool_start_idx, pool_indexes,
                               input_multiplier, input_rank, kernel_size);

          // Compute derivative value for current index
          number_t derivative_value = 0;
          dpool_algo(input_values, kernel_size, pool_indexes,
                     derivative_flat_idx, &derivative_value, pooled_flat_idx,
                     precomputed_derivatives_values
                         ? precomputed_derivatives_values[pooled_flat_idx]
                         : 1,
                     meta);

          derivative_values[derivative_flat_idx] += derivative_value;
        }

        // Update adjusted index for next pooled element
        update_index(current_pool_start_idx, input_shape, stride, NULL,
                     input_rank);
      }

      // Update derivative index
      update_index(derivative_idx, input_shape, NULL, NULL, input_rank);
    }
  }

  // Return the computed derivatives
  return derivatives;
}

#endif
