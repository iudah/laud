#ifndef LAUD_ADD_R_H
#define LAUD_ADD_R_H

#include "../../../core/var.r.h"

struct laud_add_class {
  struct laud_var_class _;
};

struct laud_add {
  struct laud_var _;
};

#define LAUD_ADD_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudAdd;
extern const void *LaudAddClass;

void *laud_narray_dadd(const struct laud_narray *operand_a,
                       const struct laud_narray *operand_b,
                       uint64_t respect_index, const struct laud_narray *pre_dx,
                       struct laud_narray *calc_result);

#endif