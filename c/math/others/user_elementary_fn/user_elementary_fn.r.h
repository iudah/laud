#ifndef LAUD_USER_ELEMENTARY_FN_R_H
#define LAUD_USER_ELEMENTARY_FN_R_H

#include "../../../core/var.r.h"
#include "../../../math/others/user_elementary_fn/user_elementary_fn.def.h"

struct laud_user_elementary_fn_class {
  struct laud_var_class _;
};

struct laud_user_elementary_fn {
  struct laud_var _;
  laud_user_elementary_fn_t user_elementary_fn;
  uint64_t no_of_operands;
  void *args;
};

#define LAUD_USER_ELEMENTARY_FN_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudUserElementaryFn;
extern const void *LaudUserElementaryFnClass;

void *laud_narray_duser_elementary_fn(const struct laud_narray *operand_a,
                                      const struct laud_narray *operand_b,
                                      uint64_t respect_index,
                                      const struct laud_narray *pre_dx,
                                      struct laud_narray *calc_result);

#endif