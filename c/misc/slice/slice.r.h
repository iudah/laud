#ifndef LAUD_SLICE_R_H
#define LAUD_SLICE_R_H

#include "../../core/var.r.h"

struct laud_slice_class {
  struct laud_var_class _;
};

struct laud_slice {
  struct laud_var _;
  struct laud_dim_slice_data *slice_data;
  struct laud_dim_slice_data *current_slice_data;
  uint64_t *limit;
  uint64_t *current_count;
  uint64_t *new_shape;
  uint64_t new_length;
};

struct laud_dim_slice_data {
  uint64_t start, stop, step, stride;
};

#define LAUD_SLICE_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudSlice;
extern const void *LaudSliceClass;

void *laud___create_slice_data(const void *array,
                               const char *const slice_format,
                               uint64_t **new_shape, uint64_t *new_length);

void *laud_narray_dslice(const struct laud_narray *operand_a,
                         const uint64_t respect_index,
                         const struct laud_narray *pre_dx,
                         const struct laud_narray *calc_result);

#endif