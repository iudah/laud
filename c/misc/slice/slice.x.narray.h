#ifndef LAUD_SLICE_X_NARRAY_H
#define LAUD_SLICE_X_NARRAY_H

#include <Ubject.h>
#include <Ubject.r.h>
#include <math.h>
#include <mem_lk.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LAUD_NARRAY_IMPLEMENTATION
#error this file must be #included in narray.c only
#endif

#define LAUD_NARRAY_IMPLEMENTATION
#include "../../core/base.h"
#include "../../core/narray.h"
#include "../../core/narray.r.static.h"
#include "../../misc/slice/slice.r.h"

static inline void adjust_slice_boundary(const uint16_t section,
                                         struct laud_dim_slice_data *slice,
                                         const int bound, const uint16_t dim,
                                         const uint64_t *shape) {
  uint64_t *current_field;

  switch (section) {
  case 0:
    current_field = &slice->start;
    break;
  case 1:
    current_field = &slice->stop;
    break;
  case 2:
    current_field = &slice->step;
    break;
  case 3:
    current_field = &slice->stride;
    break;
  default:
    UbjectError.error("assess_section: invalid section index: %i", section);
    return;
  }

  current_field[0] = (uint64_t)(bound >= 0 ? bound : (shape[dim] + bound));

  if ((section == 0 && slice->start >= shape[dim]) ||
      (section == 1 && slice->stop > shape[dim])) {
    UbjectError.error("assess_section: slice %s (%i) out of bound (%" PRIu64
                      ")",
                      section ? "end" : "start", bound, shape[dim]);
  }
}

static inline void report_expected_integer_error(const char *fmt,
                                                 const char *slice) {
  if (!fmt || !slice) {
    UbjectError.error("expected_int: invalid input in expected_int");
    return;
  }

  uint64_t info_size = 2 + (uint64_t)(fmt - slice);
  char info[info_size];
  memset(info, '~', info_size - 1);
  info[info_size - 2] = '^';
  info[info_size - 1] = 0;

  UbjectError.error("expected integer before ','\n%s\n%s", slice, info);
}

static inline void fill_slice_data_for_dimension(
    const void *self, struct laud_dim_slice_data *slice_object,
    uint64_t *new_length, uint64_t *new_shape, int16_t *dim, int16_t *section,
    char *ignore_colon) {
  const uint64_t *shape_of_self = shape(self);
  while (*section < 4) {
    uint64_t value;

    if (*section == 0) {
      value = 0;
    } else if ((*section == 1) || (*section == 3)) {
      if (*ignore_colon) {
        value = slice_object[*dim].start + 1;
      } else {
        value = shape_of_self[*dim];
      }
    } else {
      value = 1;
    }

    adjust_slice_boundary(*section, slice_object + *dim, value, *dim,
                          shape_of_self);

    (*section)++;
  }

  *new_length *= new_shape[*dim] =
      (slice_object[*dim].stop - slice_object[*dim].start - 1) /
          slice_object[*dim].step +
      1;
  *section = 0;
  (*dim)++;
  *ignore_colon = 0;
}

void *laud___create_slice_data(const void *array,
                               const char *const slice_format,
                               uint64_t **new_shape_ptr, uint64_t *new_length) {

  uint64_t *new_shape = NULL;
  if (new_shape_ptr) {
    if (*new_shape_ptr) {
      new_shape = *new_shape_ptr;
    } else {
      new_shape = *new_shape_ptr = CALLOC(rank(array), sizeof(uint64_t));
    }
  }

  char *format_cursor = (char *)slice_format;

  const uint16_t array_rank = rank(array);
  const uint64_t *array_shape = shape(array);

  struct laud_dim_slice_data *slice_object =
      CALLOC(sizeof(struct laud_dim_slice_data), array_rank);

  if (!slice_object) {
    UbjectError.error(
        "create_slice_object: memory allocation failed in laud_slice\n");
    return NULL;
  }

  char ignore_colon = 0;

  int16_t current_dimension = 0;
  int16_t current_section = 0;

  char current_char;

  while ((current_char = format_cursor[0])) {
    switch (current_char) {
    case '0' ... '9':
    case '-':
    case '+': {
      int value = (int)strtol(format_cursor, &format_cursor, 10);

      adjust_slice_boundary(current_section, slice_object + current_dimension,
                            value, current_dimension, array_shape);
      ignore_colon = 1;
      format_cursor--;
      current_section++;
    } break;

    case ':':
      if (!ignore_colon) {
        adjust_slice_boundary(
            current_section, slice_object + current_dimension,
            current_section == 0 ? 0 : array_shape[current_dimension],
            current_dimension, array_shape);
        current_section++;
      }
      ignore_colon = 0;
      break;

    case ',': {
      if (current_section == 0 || current_section == 2) {
        report_expected_integer_error(format_cursor, slice_format);
      }
      fill_slice_data_for_dimension(array, slice_object, new_length, new_shape,
                                    &current_dimension, &current_section,
                                    &ignore_colon);
    } break;

    default:
      break;
    }
    format_cursor++;
  }
  if (current_section == 0) {
    report_expected_integer_error(format_cursor, slice_format);
  }
  // current_dimension will still be less than array_rank. This while loop
  // will remedy that and complete the shape
  while (current_dimension < rank(array)) {
    fill_slice_data_for_dimension(array, slice_object, new_length, new_shape,
                                  &current_dimension, &current_section,
                                  &ignore_colon);
  }

  return slice_object;
}

static inline void apply_effective_slice(
    const uint16_t rank, const uint16_t dim,
    const struct laud_dim_slice_data *slice, const uint64_t dst_cum_offset,
    const uint64_t src_cum_offset, const uint64_t dst_dim_multiplier,
    const uint64_t src_dim_multiplier, const uint64_t *const dst_shape,
    const uint64_t *const src_shape, number_t *dest,
    const number_t *const src) {
  if (rank != dim) {

    uint64_t j = 0;
    for (uint64_t i = slice[dim].start; i < slice[dim].stop;
         i += slice[dim].step) {
      if (rank - 1 != dim) {
        apply_effective_slice(rank, dim + 1, slice,
                              dst_cum_offset + j * dst_dim_multiplier,
                              src_cum_offset + i * src_dim_multiplier,
                              dst_dim_multiplier / dst_shape[dim + 1],
                              src_dim_multiplier / src_shape[dim + 1],
                              dst_shape, src_shape, dest, src);
      } else {
        dest[dst_cum_offset + j] = src[src_cum_offset + i];
      }
      j++;
    }
  }
}

static inline void apply_slice(const struct laud_dim_slice_data *slice_object,
                               number_t *dest_values, const uint64_t *new_shape,
                               const uint64_t new_length,
                               const struct laud_narray *const unsliced_src) {
  const uint64_t *shape_of_self = shape(unsliced_src);
  apply_effective_slice(rank(unsliced_src), 0, slice_object, 0, 0,
                        new_length / new_shape[0],
                        length(unsliced_src) / shape_of_self[0], new_shape,
                        shape_of_self, dest_values, values(unsliced_src));
}

#if 0
void *laud___narray_slice_array(const struct laud_narray *array,
                                const struct laud_dim_slice_data *slice_data,
                                struct laud_narray *sliced_array) {

  // Apply the slice operation
  apply_slice(slice_data, (number_t *)values(sliced_array), shape(sliced_array),
              length(sliced_array), array);

  return sliced_array;
}
#endif

static void *narray_slice(const struct laud_narray *operand,
                          const void *slice_data, const uint64_t *slice_shape,
                          const uint64_t slice_length) {

  struct laud_narray *result = laud_narray(rank(operand), slice_shape, 0, NULL);
  number_t *result_values = values(result);

  // Apply the slice operation
  apply_slice(slice_data, result_values, slice_shape, slice_length, operand);

  return result;
}

void *laud_narray_dslice(const struct laud_narray *operand_a,
                         uint64_t __attribute__((__unused__)) respect_index,
                         const struct laud_narray *pre_dx,
                         const struct laud_narray *calc_result) {

  struct laud_narray *derivative =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  number_t *derivative_values = values(derivative);

  for (uint64_t i = 0; i < length(derivative); i++) {
    derivative_values[i] = (pre_dx ? values(pre_dx)[i] : 1) *
                           values(calc_result)[i] *
                           (1. - values(calc_result)[i]);
  }
  return derivative;
}

#endif