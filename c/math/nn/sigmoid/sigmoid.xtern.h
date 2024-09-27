#ifndef LAUD_SIGMOID_XTERN_H
#define LAUD_SIGMOID_XTERN_H

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_sigmoid(const struct laud_narray *operand_a) {

  struct laud_narray *result =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  number_t *result_values = values(result);

  for (uint64_t i = 0; i < length(result); i++) {

    result_values[i] = 1. / (1. + (number_t)exp(-values(operand_a)[i]));
  }
  return result;
}

void *laud_narray_dsigmoid(const struct laud_narray *operand_a,
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

#ifdef LAUD_VAR_IMPLEMENTATION
#include "../../../math/nn/sigmoid/sigmoid.r.h"

static void *var_sigmoid(const struct laud_var *operand_a) {

  struct laud_var *sigmoid = init(LaudSigmoid, operand_a, NULL);

  if (operand_a->value) {
    laud_evaluate_var_node(sigmoid);
  }

  return sigmoid;
}

#endif

#endif