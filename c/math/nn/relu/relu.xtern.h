#ifndef LAUD_RELU_XTERN_H
#define LAUD_RELU_XTERN_H

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_relu(const struct laud_narray *operand_a) {

  struct laud_narray *result =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  number_t *result_values = values(result);

  for (uint64_t i = 0; i < length(result); i++) {

    result_values[i] = values(operand_a)[i];
    if (result_values[i] < 0)
      result_values[i] = 0;
  }
  return result;
}

void *laud_narray_drelu(const struct laud_narray *operand_a,
                        uint64_t __attribute__((__unused__)) respect_index,
                        struct laud_narray *pre_dx,
                        struct laud_narray *calc_result) {

  struct laud_narray *derivative =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  number_t *derivative_values = values(derivative);

  for (uint64_t i = 0; i < length(derivative); i++) {
    derivative_values[i] =
        values(calc_result)[i] == 0 ? 0 : (pre_dx ? values(pre_dx)[i] : 1);
  }
  return derivative;
}

#endif

#ifdef LAUD_VAR_IMPLEMENTATION
#include "../../../math/nn/relu/relu.r.h"

static void *var_relu(const struct laud_var *operand_a) {

  struct laud_var *relu = init(LaudReLU, operand_a, NULL);

  if (operand_a->value) {
    laud_evaluate_var_node(relu);
  }

  return relu;
}

#endif

#endif