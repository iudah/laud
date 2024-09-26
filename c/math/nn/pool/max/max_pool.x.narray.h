#ifndef LAUD_MAX_POOL_X_NARRAY_H
#define LAUD_MAX_POOL_X_NARRAY_H

#include <Ubject.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef LAUD_NARRAY_IMPLEMENTATION
#error this must be #included in narray.c only
#define LAUD_NARRAY_IMPLEMENTATION
#endif

#include "../../../../core/narray.h"
#include "../../../../core/narray.r.static.h"

void *laud_narray_dmaxpool(const struct laud_narray *input_image,
                           const uint64_t respect_flag,
                           const struct laud_narray *precomputed_derivatives,
                           const struct laud_narray *pooled_result) {

  // Fetch input image properties
  const uint64_t input_rank = rank(input_image);
  const uint64_t *input_shape = shape(input_image);
  const uint64_t output_size = length(pooled_result);

  const uint64_t *padding_info = get_meta_data(pooled_result);
  const uint64_t *stride = padding_info + 2 * input_rank;
  const uint64_t *filter_shape = stride + input_rank;
  const uint64_t *max_indexes = *(uint64_t **)(filter_shape + input_rank);

  // Fetch kernel properties
  const uint64_t kernel_rank = input_rank;
  // Todo: remove debug info
  //  Debug: print input rank
  printf(respect_flag ? "Kernel Rank: %" PRIu64 "\n"
                      : "Input Rank: %" PRIu64 "\n",
         respect_flag ? kernel_rank : input_rank);

  // uint64_t slide_window[input_rank];
  // memset(slide_window, 0, sizeof(slide_window));

  struct laud_narray *derivatives = NULL;

  // Fetch precomputed derivatives if available
  const number_t *precomputed_derivatives_values =
      precomputed_derivatives ? values(precomputed_derivatives) : NULL;

  if (respect_flag == 0) {

    const uint64_t derivative_rank = input_rank;
    const uint64_t *derivative_shape = input_shape;
    derivatives = laud_narray(derivative_rank, derivative_shape, 0, NULL);
    number_t *derivative_values = values(derivatives);

    for (uint64_t idx = 0; idx < output_size; idx++) {
      derivative_values[max_indexes[idx]] +=
          (precomputed_derivatives ? precomputed_derivatives_values[idx] : 1);
      ;
    }
  }

  char buffer[2048];
  laud_to_string(derivatives, buffer, 2048);
  puts(buffer);

  // Return the derivative
  return derivatives;
}

#endif