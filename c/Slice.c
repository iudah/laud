#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <Ubject.h>
#include <string.h>

#include "Var.h"
#include "Var.r.h"

#define LAUD_SLICE_PRIORITY (LAUD_VAR_PRIORITY + 2)
#define SLICE_C
#include "Slice.r.h"
#undef SLICE_C

static void apply_slice(const struct LaudSliceObject *slice_object,
                        float *values, const size_t *new_shape,
                        const size_t new_length,
                        const struct LaudVar *const self);

static void apply_effective_slice(
    const size_t rank, const size_t dim, const struct LaudSliceObject *slice,
    const size_t dst_cum_offset, const size_t src_cum_offset,
    const size_t dst_dim_multiplier, const size_t src_dim_multiplier,
    const size_t *const dst_shape, const size_t *const src_shape, float *dest,
    const float *const src);

void *laud_slice(const void *input_var, const char *slice_fmt, ...) {
  size_t input_rank = laud_rank(input_var);
  size_t new_shape[input_rank];
  size_t new_length = 1;

  va_list args;
  va_start(args, slice_fmt);

  struct LaudSliceObject *slice_object =
      create_slice_object(input_var, slice_fmt, &args, new_shape, &new_length);
  va_end(args);

  struct LaudVar *sliced_var =
      init(LaudSlice, input_rank, new_shape, 0, NULL, input_var, slice_object);

  laud_evaluate(sliced_var);

  return sliced_var;
}

static void adjust_slice_boundary(const size_t section,
                                  struct LaudSliceObject *slice,
                                  const int bound, const size_t dim,
                                  const size_t *shape) {
  size_t *current_field;

  switch (section) {
  case 0:
    current_field = &slice->start;
    break;
  case 1:
    current_field = &slice->end;
    break;
  case 2:
    current_field = &slice->step;
    break;
  case 3:
    current_field = &slice->stride;
    break;
  default:
    UbjectError.error("assess_section: invalid section index: %zu", section);
    return;
  }

  current_field[0] = (size_t)(bound >= 0 ? bound : (shape[dim] + bound));

  if ((section == 0 && slice->start >= shape[dim]) ||
      (section == 1 && slice->end > shape[dim])) {
    UbjectError.error("assess_section: slice %s (%i) out of bound (%i)",
                      section ? "end" : "start", bound, shape[dim]);
  }
}
static void report_expected_integer_error(const char *fmt, const char *slice) {
  if (!fmt || !slice) {
    UbjectError.error("expected_int: invalid input in expected_int");
    return;
  }

  size_t info_size = 2 + (size_t)(fmt - slice);
  char info[info_size];
  memset(info, '~', info_size - 1);
  info[info_size - 2] = '^';
  info[info_size - 1] = 0;

  UbjectError.error("expected integer before ','\n%s\n%s", slice, info);
}
static void apply_slice(const struct LaudSliceObject *slice_object,
                        float *values, const size_t *new_shape,
                        const size_t new_length,
                        const struct LaudVar *const self) {
  const size_t *shape_of_self = laud_shape(self);
  apply_effective_slice(laud_rank(self), 0, slice_object, 0, 0,
                        new_length / new_shape[0],
                        laud_length(self) / shape_of_self[0], new_shape,
                        shape_of_self, values, laud_values(self));
}
static void apply_effective_slice(
    const size_t rank, const size_t dim, const struct LaudSliceObject *slice,
    const size_t dst_cum_offset, const size_t src_cum_offset,
    const size_t dst_dim_multiplier, const size_t src_dim_multiplier,
    const size_t *const dst_shape, const size_t *const src_shape, float *dest,
    const float *const src) {
  if (rank != dim) {

    size_t j = 0;
    for (size_t i = slice[dim].start; i < slice[dim].end;
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

static inline void fill_slice_data_for_dimension(
    const void *self, struct LaudSliceObject *slice_object, size_t *new_length,
    size_t *new_shape, size_t *dim, size_t *section, char *ignore_colon) {
  const size_t *shape_of_self = laud_shape(self);
  while (*section < 4) {
    size_t value;

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
      (slice_object[*dim].end - slice_object[*dim].start - 1) /
          slice_object[*dim].step +
      1;
  *section = 0;
  (*dim)++;
  *ignore_colon = 0;
}

void *create_slice_object(const void *array, const char *const slice_format,
                          va_list *format_args, size_t *new_shape,
                          size_t *new_length) {
  char *format_cursor = (char *)slice_format;

  const size_t array_rank = laud_rank(array);
  const size_t *array_shape = laud_shape(array);

  struct LaudSliceObject *slice_object =
      malloc(sizeof(struct LaudSliceObject) * array_rank);

  if (!slice_object) {
    UbjectError.error("laud_slice: memory allocation failed in laud_slice\n");
    return NULL;
  }

  char ignore_colon = 0;

  size_t current_dimension = 0;
  size_t current_section = 0;

  char current_char;

  while ((current_char = format_cursor[0])) {
    switch (current_char) {
    case '0' ... '9':
    case '-':
    case '+':
    case '.':
    case '%': {
      int value = 0;

      if (current_char == '%') {
        format_cursor++;

        if (format_cursor[0] == 'i') {
          value = va_arg(*format_args, int);
        } else {
          UbjectError.error(
              "make_slice_object: only %%i identifier is accepted\n");
        }

      } else {
        value = (int)strtol(format_cursor, &format_cursor, 10);
      }

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
  // current_dimension will still be less than array_rank. This while loop will
  // remedy that and complete the shape
  while (current_dimension < laud_rank(array)) {
    fill_slice_data_for_dimension(array, slice_object, new_length, new_shape,
                                  &current_dimension, &current_section,
                                  &ignore_colon);
  }

  return slice_object;
}

static const void *laud_slice_evaluate(void *self) {
  struct LaudSlice *slice_instance = self;

  // Apply the slice operation
  apply_slice(slice_instance->slice_info, (float *)laud_values(slice_instance),
              laud_shape(slice_instance), laud_length(slice_instance),
              slice_instance->dependency_var);

  return slice_instance;
}

static const void *laud_slice_differentiate(void *self) {
  struct LaudSlice *this = self;

  // Issue a warning about not implementing differentiation
  UbjectError.warn("No differentiation implemented for %p\n", this);

  return NULL;
}

static void *laud_slice_ctor(void *self, va_list *args) {
  // Call the superclass constructor
  struct LaudSlice *this = super_ctor(LaudSlice, self, args);

  // Extract arguments from the variable arguments list
  this->dependency_var = va_arg(*args, struct LaudVar *);
  this->slice_info = va_arg(*args, struct LaudSliceObject *);

  return this;
}

const struct LaudSliceObject *get_slice_info(const struct LaudSlice *self) {
  return self->slice_info;
}

const struct LaudVar *get_slice_dependency(const struct LaudSlice *self) {
  return self->dependency_var;
}

const void *LaudSlice = NULL;
static void __attribute__((constructor(LAUD_SLICE_PRIORITY)))
initLaudShuffle(void) {
  // Initialize LaudSlice if not already initialized
  if (!LaudSlice) {
    LaudSlice = init(LaudVarClass, LaudVar, sizeof(struct LaudSlice), ctor,
                     laud_slice_ctor, className, "LaudSlice", laud_evaluate,
                     laud_slice_evaluate, laud_differentiate,
                     laud_slice_differentiate, NULL);
  }
}
