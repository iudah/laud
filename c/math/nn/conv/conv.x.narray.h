#ifndef LAUD_CONV_X_NARRAY_H
#define LAUD_CONV_X_NARRAY_H

#include <Ubject.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "BaseObject.h"
#include "mem_lk.h"

#ifndef LAUD_NARRAY_IMPLEMENTATION
#error this must be #included in narray.c only
#define LAUD_NARRAY_IMPLEMENTATION
#endif

#include "../../../core/narray.h"
#include "../../../core/narray.r.static.h"
#include "../../../core/narray_bc.r.static.h"
#include "../../../math/nn/conv_pool_static_fn.h"

static void perform_convolution_step(
    number_t *output_values, const uint64_t output_flat_index,
    const uint64_t *output_index, const number_t *image_values,
    const number_t *filter_values, uint64_t *image_index,
    uint64_t *filter_index, const uint64_t *image_multiplier,
    const uint64_t *filter_multiplier, uint64_t *sub_filter_index,
    const uint64_t *sub_filter_shape, const uint64_t sub_filter_length,
    const uint64_t *image_shape, const uint64_t image_rank,
    const uint64_t *stride, const uint64_t *active_paddings,
    const uint64_t filter_image_rank_diff) {

  // Initialize output to 0 for this convolution step
  output_values[output_flat_index] = 0;

  // Initialize output to 0 for this convolution step
  for (uint64_t i = 0; i < filter_image_rank_diff; i++) {
    filter_index[i] = output_index[i];
  }

  // Calculate starting index in the image by applying strides and subtracting
  // padding
  const uint64_t *matched_output_to_image =
      output_index + filter_image_rank_diff;

  uint64_t image_start_index[image_rank];
  for (uint64_t i = 0; i < image_rank; i++) {
    image_index[i] = image_start_index[i] =
        (int64_t)matched_output_to_image[i] * (int64_t)stride[i] -
        (int64_t)active_paddings[i * 2];
  }

  // Iterate through filter elements (sub_filter)
  memset(sub_filter_index, 0, image_rank * sizeof(*sub_filter_index));
  for (uint64_t iter = 0; iter < sub_filter_length; iter++) {
    // Check whether the image index is within bounds
    if (!should_skip_conv_pool_operation(image_index, image_shape,
                                         image_rank)) {
      // Accumulate convolution result by computing flat indices for image and
      // filter
      uint64_t img_flat_idx =
          compute_flat_index(image_index, image_multiplier, image_rank);
      uint64_t filt_flat_idx =
          compute_flat_index(filter_index, filter_multiplier, image_rank);

      output_values[output_flat_index] +=
          image_values[img_flat_idx] * filter_values[filt_flat_idx];
    }

    // Update image and filter indices for the next sub_filter iteration
    update_image_and_filter_indices(image_start_index, image_index,
                                    sub_filter_index, sub_filter_shape,
                                    image_rank);
  }
}

static void *narray_conv(const struct laud_narray *input_image,
                         const struct laud_narray *filters, uint64_t *stride,
                         uint64_t *padding) {

  // Initialize image variables
  const uint64_t image_rank = rank(input_image);
  const uint64_t *image_shape = shape(input_image);
  uint64_t image_index[image_rank];
  number_t *image_values = values(input_image);
  uint64_t image_multiplier[image_rank];
  compute_multiplier(image_multiplier, image_shape, image_rank);

  // Initialize filter variables
  const uint64_t filter_rank = rank(filters);
  const uint64_t *filter_shape = shape(filters);
  number_t *filter_values = values(filters);
  uint64_t filter_index[filter_rank];
  uint64_t filter_multiplier[filter_rank];
  compute_multiplier(filter_multiplier, filter_shape, filter_rank);

  // Ensure the image rank is less than or equal to the filter rank
  if (image_rank > filter_rank) {
    UbjectError.error("image rank must be less than or equal to filter rank");
    return NULL;
  }

  // Handle padding and stride
  uint64_t *active_paddings = CALLOC(3 * image_rank, sizeof(uint64_t));
  uint64_t *active_strides = active_paddings + 2 * sizeof(*active_paddings);
  initialize_padding_stride(&stride, padding, active_paddings, image_rank);

  // Compute output shape and create output narray
  uint64_t output_shape[filter_rank];
  uint64_t output_index[filter_rank];
  compute_conv_pool_output_shape(image_shape, filter_shape, output_shape,
                                 image_rank, filter_rank, stride,
                                 active_paddings);
  struct laud_narray *output = laud_narray(filter_rank, output_shape, 0, NULL);
  number_t *output_values = values(output);
  set_meta_data(output, active_paddings);

  // Perform convolution
  const uint64_t filter_image_rank_diff = filter_rank - image_rank;
  const uint64_t *sub_filter_shape = filter_shape + filter_image_rank_diff;
  uint64_t *sub_filter_index = filter_index + filter_image_rank_diff;
  const uint64_t sub_filter_length =
      compute_length_from_shape(sub_filter_shape, image_rank);

  uint64_t output_size = length(output);
  for (uint64_t output_flat_index = 0; output_flat_index < output_size;
       output_flat_index++) {

    perform_convolution_step(
        output_values, output_flat_index, output_index, image_values,
        filter_values, image_index, filter_index, image_multiplier,
        filter_multiplier, sub_filter_index, sub_filter_shape,
        sub_filter_length, image_shape, image_rank, active_strides,
        active_paddings, filter_image_rank_diff);
    update_index(output_index, output_shape, NULL, NULL, filter_rank);
  }

  return output;
}

static char should_compute_convolution_derivative(
    const uint64_t *input_idx, const uint64_t *spatial_kernel_idx,
    const uint64_t *shape, const uint64_t *spatial_kernel_shape,
    const uint64_t *stride, const uint64_t rank) {
  for (uint64_t i = 0; i < rank; i++) {
    if ((input_idx[i] - spatial_kernel_idx[i] + spatial_kernel_shape[i] >
         shape[i]) ||
        ((input_idx[i] - spatial_kernel_idx[i]) / stride[i] >
         (spatial_kernel_shape[i] - 1))) {
      return 0;
    }
  }
  return 1;
}

static inline char update_multi_dim_index(uint64_t *index,
                                          const uint64_t *shape,
                                          const uint64_t rank) {
  return update_index(index, shape, NULL, NULL, rank);
}

void *laud_narray_dconv(const struct laud_narray *input_image,
                        const struct laud_narray *kernel_filters,
                        const uint64_t respect_flag,
                        const struct laud_narray *precomputed_derivatives,
                        const struct laud_narray *convolution_image) {

  // Fetch input image properties
  const uint64_t input_rank = rank(input_image);
  const uint64_t *input_shape = shape(input_image);
  const number_t *input_values = values(input_image);
  uint64_t input_idx[input_rank], input_start_idx[input_rank],
      input_multiplier[input_rank];
  compute_multiplier(input_multiplier, input_shape, input_rank);

  // Fetch kernel properties
  const uint64_t kernel_rank = rank(kernel_filters);
  const uint64_t *kernel_shape = shape(kernel_filters);
  const number_t *kernel_values = values(kernel_filters);

  const uint64_t non_spatial_rank = kernel_rank - input_rank;
  uint64_t non_spatial_idx[non_spatial_rank];
  memset(non_spatial_idx, 0, sizeof(non_spatial_idx));

  uint64_t spatial_kernel_idx[input_rank];
  uint64_t kernel_multiplier[kernel_rank];
  compute_multiplier(kernel_multiplier, kernel_shape, kernel_rank);

  // Define multipliers
  const uint64_t *spatial_kernel_multiplier =
      kernel_multiplier + non_spatial_rank;
  const uint64_t *non_spatial_shape = kernel_shape;

  // Fetch convolution image properties
  const uint64_t *convolution_shape = shape(convolution_image);
  const uint64_t convolution_rank = rank(convolution_image);

  uint64_t spatial_convolution_idx[input_rank];
  memset(spatial_convolution_idx, 0, sizeof(spatial_convolution_idx));

  uint64_t convolution_multiplier[convolution_rank];
  compute_multiplier(convolution_multiplier, convolution_shape,
                     convolution_rank);
  const uint64_t *spatial_convolution_multiplier =
      convolution_multiplier + non_spatial_rank;

  const uint64_t *padding_info = get_meta_data(convolution_image);
  const uint64_t *stride = padding_info + 2 * input_rank;

  // Initialize derivative array
  struct laud_narray *derivatives = NULL;
  const number_t *precomputed_derivatives_values =
      precomputed_derivatives ? values(precomputed_derivatives) : NULL;

  // Select whether to compute derivatives w.r.t. input image or kernel
  const uint64_t derivative_rank = respect_flag == 0 ? input_rank : kernel_rank;
  const uint64_t *derivative_shape =
      respect_flag == 0 ? input_shape : kernel_shape;
  derivatives = laud_narray(derivative_rank, derivative_shape, 0, NULL);
  number_t *derivative_values = values(derivatives);

  uint64_t derivative_idx[derivative_rank];
  memset(derivative_idx, 0, sizeof(derivative_idx));

  const uint64_t length_derivative =
      respect_flag == 0 ? length(input_image) : length(kernel_filters);

  for (uint64_t derivative_flat_idx = 0;
       derivative_flat_idx < length_derivative; derivative_flat_idx++) {
    derivative_values[derivative_flat_idx] = 0;

    if (respect_flag == 0) {
      // Respect flag is zero, compute w.r.t. input image
      memset(spatial_kernel_idx, 0, sizeof(spatial_kernel_idx));
      uint64_t spatial_kernel_initial_idx[input_rank];

      for (int64_t i = input_rank - 1; i >= 0; i--) {
        spatial_kernel_initial_idx[i] = spatial_kernel_idx[i] =
            derivative_idx[i] % stride[i];
      }

      char iter = 1;
      while (iter) {
        if (should_compute_convolution_derivative(
                derivative_idx, spatial_kernel_idx, input_shape,
                kernel_shape + non_spatial_rank, stride, input_rank)) {

          char iter_non_spatial = 1;
          while (iter_non_spatial) {
            // Accumulate derivative values
            derivative_values[derivative_flat_idx] +=
                kernel_values[(non_spatial_rank
                                   ? compute_flat_index(non_spatial_idx,
                                                        kernel_multiplier,
                                                        non_spatial_rank)
                                   : 0) +
                              compute_flat_index(spatial_kernel_idx,
                                                 spatial_kernel_multiplier,
                                                 input_rank)] *
                (precomputed_derivatives
                     ? precomputed_derivatives_values
                           [(non_spatial_rank
                                 ? compute_flat_index(non_spatial_idx,
                                                      convolution_multiplier,
                                                      non_spatial_rank)
                                 : 0) +
                            compute_flat_index(spatial_convolution_idx,
                                               spatial_convolution_multiplier,
                                               input_rank)]
                     : 1);

            // Update non-spatial index
            if (non_spatial_rank && (non_spatial_rank < kernel_rank)) {
              iter_non_spatial = update_multi_dim_index(
                  non_spatial_idx, non_spatial_shape, non_spatial_rank);
            } else {
              iter_non_spatial = 0;
            }
          }
        }

        // Update spatial kernel indices for the next iteration
        iter = update_index(spatial_kernel_idx, kernel_shape + non_spatial_rank,
                            stride, spatial_kernel_initial_idx, input_rank);
      }

    } else {
      // Respect flag is set, compute w.r.t. kernel
      if (non_spatial_rank &&
          !(derivative_flat_idx %
            compute_length_from_shape(kernel_shape + non_spatial_rank,
                                      input_rank))) {
        non_spatial_idx[0] = derivative_flat_idx;
      }

      memcpy(input_start_idx, spatial_kernel_idx, sizeof(input_start_idx));
      memcpy(input_idx, spatial_kernel_idx, sizeof(input_idx));

      char iter = 1;
      while (iter) {
        if (should_compute_convolution_derivative(
                input_idx, spatial_kernel_idx, input_shape,
                kernel_shape + non_spatial_rank, stride, input_rank)) {

          for (uint64_t dim = 0; dim < input_rank; dim++) {
            spatial_convolution_idx[dim] = input_idx[dim] / stride[dim];
          }

          // Accumulate derivative values
          derivative_values[derivative_flat_idx] +=
              input_values[compute_flat_index(input_idx, input_multiplier,
                                              input_rank)] *
              (precomputed_derivatives
                   ? precomputed_derivatives_values
                         [non_spatial_idx[0] +
                          compute_flat_index(spatial_convolution_idx,
                                             spatial_convolution_multiplier,
                                             input_rank)]
                   : 1);
        }

        // Update input indices for next iteration
        iter = update_index(input_idx, input_shape, stride, input_start_idx,
                            input_rank);
      }
    }

    // Increment derivative index
    update_multi_dim_index(derivative_idx, derivative_shape, derivative_rank);
  }

  return derivatives;
}

#endif
