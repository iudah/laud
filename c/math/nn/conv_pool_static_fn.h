#ifndef LAUD_CONV_POOL_STATIC_FN_H
#define LAUD_CONV_POOL_STATIC_FN_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void initialize_stride(uint64_t **stride, uint64_t *default_stride,
                              uint64_t image_rank) {
  for (uint64_t i = 0; i < image_rank; i++) {
    default_stride[i] = 1;
  }
  *stride = default_stride;
}

static uint64_t compute_length_from_shape(const uint64_t *shape_array,
                                          const uint64_t shape_rank) {
  uint64_t length = 1;
  for (uint64_t i = 0; i < shape_rank; i++) {
    length *= shape_array[i];
  }
  return length;
}

static void compute_conv_pool_output_shape(
    const uint64_t *image_shape, const uint64_t *filter_shape,
    uint64_t *output_shape, uint64_t image_rank, uint64_t filter_rank,
    const uint64_t *stride, const uint64_t *paddings) {
  uint64_t img_dim = image_rank;
  uint64_t filt_dim = filter_rank;

  // Compute the output shape for the dimensions that match between image and
  // filter
  while (img_dim && filt_dim) {
    img_dim--;
    filt_dim--;
    output_shape[filt_dim] =
        ((image_shape[img_dim] + paddings[img_dim * 2] +
          paddings[img_dim * 2 + 1] - filter_shape[filt_dim]) /
         stride[img_dim]) +
        1;
  }

  // Set the remaining dimensions of the output shape to match the filter shape
  while (filt_dim) {
    filt_dim--;
    output_shape[filt_dim] = filter_shape[filt_dim];
  }
}

static void compute_multiplier(uint64_t *multiplier, const uint64_t *shape,
                               uint64_t rank) {
  uint64_t i = rank - 1;
  multiplier[i] = 1;

  for (--i; i >= 0 && i < rank; i--) {
    multiplier[i] = multiplier[i + 1] * shape[i + 1];
  }
}

static void print_index(const uint64_t *index, uint64_t rank, char is_signed) {
  putchar('(');
  for (uint64_t i = 0; i < rank; i++) {
    printf(is_signed ? " %" PRId64 : " %" PRIu64, index[i]);
  }
  putchar(')');
}

static uint64_t compute_flat_index(const uint64_t *index,
                                   const uint64_t *multiplier,
                                   const uint64_t rank) {
  uint64_t output = 0;
  for (uint64_t i = 0; i < rank; i++) {
    output += index[i] * multiplier[i];
  }
  return output;
}
static int should_skip_conv_pool_operation(const uint64_t *image_index,
                                           const uint64_t *image_shape,
                                           uint64_t image_rank) {
  for (uint64_t i = 0; i < image_rank; i++) {
    if (image_index[i] >= image_shape[i]) {
      return 1; // Skip if index is out of bounds
    }
  }
  return 0;
}
static void update_image_and_filter_indices(const uint64_t *image_start_index,
                                            uint64_t *image_index,
                                            uint64_t *sub_filter_index,
                                            const uint64_t *sub_filter_shape,
                                            uint64_t image_rank) {
  uint64_t dim = image_rank - 1;
  image_index[dim] = image_start_index[dim] + ++sub_filter_index[dim];
  while (sub_filter_index[dim] == sub_filter_shape[dim]) {

    image_index[dim] = image_start_index[dim] + (sub_filter_index[dim] = 0);
    if (!dim)
      break;
    --dim;
    image_index[dim] = image_start_index[dim] + ++sub_filter_index[dim];
  }
}

static uint64_t update_index(uint64_t *idx, const uint64_t *shape,
                             const uint64_t *stride,
                             const uint64_t *starting_index, uint64_t rank) {

  uint64_t dim = rank - 1;
  // Increment the last dimension by stride
  if (stride)
    idx[dim] += stride[dim];
  else
    idx[dim]++;

  // Handle overflow of adjusted index
  while (idx[dim] >= shape[dim]) {
    // Reset current dimension
    if (!starting_index)
      idx[dim] = 0;
    else
      idx[dim] = starting_index[dim];

    if (dim == 0)
      break; // If we have iterated through all dimensions
    dim--;
    // Increment the last dimension by stride
    if (stride)
      idx[dim] += stride[dim];
    else
      idx[dim]++;
  }
  return dim;
}

// static void update_output_index(uint64_t *output_index, uint64_t
// *filter_index,
//                                 const uint64_t *output_shape,
//                                 const uint64_t filter_rank,
//                                 uint64_t image_rank) {
//   uint64_t output_dim = filter_rank - 1;
//   output_index[output_dim]++;
//   while (output_index[output_dim] == output_shape[output_dim]) {

//     output_index[output_dim] = 0;
//     if (!output_dim)
//       break;
//     output_index[--output_dim]++;
//     if (output_dim < (filter_rank - image_rank)) {
//       filter_index[output_dim] = output_index[output_dim];
//     }
//   }
// }

static void initialize_padding_stride(uint64_t **stride, uint64_t *padding,
                                      uint64_t *active_paddings,
                                      uint64_t image_rank) {
  uint64_t *default_stride = active_paddings + 2 * image_rank;
  if (padding) {
    memcpy(active_paddings, padding, 2 * image_rank * sizeof(uint64_t));
  }
  if (*stride == NULL) {
    initialize_stride(stride, default_stride, image_rank);
  } else {
    memcpy(default_stride, *stride, image_rank * sizeof(uint64_t));
  }
}

#endif
