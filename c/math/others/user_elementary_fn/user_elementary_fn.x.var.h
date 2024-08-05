#ifndef USER_ELEMENTARY_FN_X_VAR_H
#define USER_ELEMENTARY_FN_X_VAR_H

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include <Ubject.h>

#include "../../../core/var.r.h"
#include "../../../math/others/user_elementary_fn/user_elementary_fn.r.h"

static void *var_user_elementary_fn(const struct laud_var *operand_a,
                                    const struct laud_var *operand_b) {
  abort();
  struct laud_var *user_elementary_fnition =
      init(LaudUserElementaryFn, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    // laud_evaluate_var_node(user_elementary_fnition);
  }

  return user_elementary_fnition;
}

#endif
