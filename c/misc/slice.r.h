#ifndef SLICE_R_H
#define SLICE_R_H

#include "../core/var.r.h"

struct laud_slice_class {
  struct laud_var_class _;
};

struct laud_slice {
  struct laud_var _;
  const char *slice_format_string;
};

#define LAUD_SLICE_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudSlice;
extern const void *LaudSliceClass;

#ifdef LAUD_SLICE_IMPL

struct laud_dim_slice_data {
  size_t start, stop, step, stride;
};

void *laud___create_slice_data_(const void *array,
                                const char *const slice_format,
                                size_t *new_shape, size_t *new_length);

void *laud___narray_slice_array_(const struct laud_narray *array,
                                 const struct laud_dim_slice_data *slice_data,
                                 struct laud_narray *sliced_array);
#endif

struct laud_slicer_class {
  struct TypeClass _;
};

struct laud_slicer {
  struct Ubject _;
  void *src_narray;
  const char *slice_format_string;
  struct laud_dim_slice_data *slice_data;
  struct laud_dim_slice_data *current_slice_data;
  size_t *limit;
  size_t *current_count;
  size_t *new_shape;
  size_t new_length;
};

#endif