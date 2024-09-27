#ifndef LAUD_ADD_XTERN_H
#define LAUD_ADD_XTERN_H

#include <inttypes.h>
#include <stdint.h>

#include <Ubject.h>

#include "../../../core/var.r.static.h"
#include "../../../math/common/add/add.r.h"

static void *var_add(const struct laud_var *operand_a,
                     const struct laud_var *operand_b) {

  struct laud_var *addition = init(LaudAdd, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    laud_evaluate_var_node(addition);
  }

  return addition;
}

#endif
