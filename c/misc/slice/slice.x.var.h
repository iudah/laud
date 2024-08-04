#ifndef SLICE_X_VAR_H
#define SLICE_X_VAR_H

#include <Ubject.h>

#define VAR_PROTECTED
#include "../../core/var.r.h"
#include "../../misc/slice/slice.r.h"

// static void *var_slice(const struct laud_var *operand, const void *
// slice_data, const void *slice_shape, const);

static void *var_slice(const struct laud_var *operand, const void *slice_data,
                       const uint64_t *slice_shape,
                       const uint64_t slice_length) {

  struct laud_var *slice = init(LaudSlice, operand, /*operand_b*/ NULL,
                                slice_data, slice_shape, slice_length, NULL);

  if (operand->value) {
    laud_evaluate_var_node(slice);
  }

  return slice;
}

#endif