#ifndef MATRIX_DOT_XTERN_H
#define MATRIX_DOT_XTERN_H

#include <inttypes.h>
#include <stdint.h>

#include <Ubject.h>

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_matrix_dot(const struct laud_narray *operand_a,
                               const struct laud_narray *operand_b) {
  if (rank(operand_a) != 2) {
    UbjectError.error("rank of operand 1 != 2");
  }
  if (rank(operand_b) != 2) {
    UbjectError.error("rank of operand 2 != 2");
  }
  if (shape(operand_a)[1] != shape(operand_b)[0]) {
    UbjectError.error("matrices dims mismatch; %" PRIu64 " != %" PRIu64 "",
                      shape(operand_a)[1], shape(operand_b)[0]);
  }

  uint64_t result_shape[] = {shape(operand_a)[0], shape(operand_b)[1]};
  uint64_t common = shape(operand_a)[1];

  struct laud_narray *result = laud_narray(2, result_shape, 0, NULL);
  number_t *result_values = values(result);

  for (uint64_t i = 0; i < result_shape[0]; i++) {

    for (uint64_t j = 0; j < result_shape[1]; j++) {

      for (uint64_t k = 0; k < common; k++) {
        result_values[i * result_shape[1] + j] +=
            (values(operand_a)[i * common + k] *
             values(operand_b)[k * result_shape[1] + j]);
      }
    }
  }
  return result;
}

void *laud_narray_dmatrix_dot(const struct laud_narray *operand_a,
                              const struct laud_narray *operand_b,
                              uint64_t respect_index,
                              struct laud_narray *pre_dx,
                              struct laud_narray *calc_result) {

  struct laud_narray *derivatives = NULL;

  if (respect_index == 0) {
    const struct laud_narray *operand = operand_a;
    const uint64_t *shape_operand = shape(operand);
    const uint64_t *shape_multiplicand = shape(operand_b);
    const uint64_t *shape_pre_dx = shape(pre_dx);

    derivatives = laud_narray(2, shape_operand, 0, NULL);

    number_t *derivative_values = values(derivatives);
    const number_t *multiplicand_values = values(operand_b);
    const number_t *pre_dx_values = values(pre_dx);
    // i - kk - j i - j
    //  i - jk - j i - k

    // const uint64_t common = shape(pre_dx)[1];

    for (uint64_t i = 0; i < shape_pre_dx[0]; i++) {

      for (uint64_t k = 0; k < shape_pre_dx[1]; k++) {

        for (uint64_t j = 0; j < shape_multiplicand[0]; j++) {

          derivative_values[i * shape_operand[1] + j] +=
              pre_dx_values[i * shape_pre_dx[1] + k] *
              multiplicand_values[j * shape_multiplicand[1] + k];
        }
      }
    }
  } else {
    const struct laud_narray *operand = operand_b;
    const uint64_t *shape_operand = shape(operand);
    const uint64_t *shape_multiplicand = shape(operand_a);
    const uint64_t *shape_pre_dx = shape(pre_dx);

    derivatives = laud_narray(2, shape_operand, 0, NULL);

    number_t *derivative_values = values(derivatives);
    const number_t *multiplicand_values = values(operand_a);
    const number_t *pre_dx_values = values(pre_dx);
    // i - kk - j i - j
    //  i - jk - j i - k

    // const uint64_t common = shape(pre_dx)[1];

    for (uint64_t i = 0; i < shape_pre_dx[0]; i++) {

      for (uint64_t k = 0; k < shape_pre_dx[1]; k++) {

        for (uint64_t j = 0; j < shape_multiplicand[1]; j++) {

          derivative_values[j * shape_operand[1] + k] +=
              pre_dx_values[i * shape_pre_dx[1] + k] *
              multiplicand_values[i * shape_multiplicand[1] + j];
        }
      }
    }
  }
  return derivatives;
}
#endif

#ifdef LAUD_VAR_IMPLEMENTATION

#include "../../../math/common/matrix_dot/matrix_dot.r.h"

static void *var_matrix_dot(const struct laud_var *operand_a,
                            const struct laud_var *operand_b) {

  struct laud_var *dot_product =
      init(LaudMatrixDot, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    laud_evaluate_var_node(dot_product);
  }

  return dot_product;
}

#endif

#endif