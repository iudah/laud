#ifndef ADD_XTERN_H
#define ADD_XTERN_H

#include <inttypes.h>
#include <stdint.h>

#include <Ubject.h>

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_add(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b) {

  struct laud_element_broadcast broadcast_element = {
      .multiplier_a = NULL,
      .multiplier_b = NULL,
      .shape = NULL,
      .rank = 0,
  };

  element_broadcast((struct laud_element_broadcast *)&broadcast_element,
                    operand_a, operand_b);

  void *broadcast_shape___ = broadcast_element.shape;

  if (!broadcast_shape___) {
    uint16_t rank_ = rank(operand_a);
    uint64_t *shape_ = shape(operand_a);
    uint64_t lenght_ = length(operand_a);

    struct laud_narray *result = laud_narray(rank_, shape_, 0, NULL);
    float *result_values = values(result);

    for (uint64_t i = 0; i < lenght_; i++) {

      result_values[i] = (values(operand_a)[i] + values(operand_b)[i]);
    }
    return result;
  } else {
    uint16_t rank_a = rank(operand_a);
    uint16_t rank_b = rank(operand_b);
    uint16_t rank_bc = broadcast_element.rank;

    uint64_t *shape_a = shape(operand_a);
    uint64_t *shape_b = shape(operand_b);

    struct laud_narray *result = laud_narray_bc(
        broadcast_element.rank, broadcast_element.shape, 0, NULL,
        broadcast_element.multiplier_a, broadcast_element.multiplier_b);
    volatile float *result_values = values(result);

    uint64_t length_bc = length(result);

    uint64_t index[rank_bc];
    memset(index, 0, sizeof(index));

    for (uint64_t i = 0; i < length_bc; i++) {

      uint64_t index_a = 0;
      uint64_t index_b = 0;
      uint16_t j = 0;
      while (j < rank_a && j < rank_b) {
        j += (uint16_t)1;
        index_a += shape_a[rank_a - j] > 1
                       ? broadcast_element.multiplier_a[rank_a - j] *
                             index[rank_bc - j]
                       : 0;

        index_b += shape_b[rank_b - j] > 1
                       ? broadcast_element.multiplier_b[rank_b - j] *
                             index[rank_bc - j]
                       : 0;
      }
      while (j < rank_a) {
        j++;
        index_a +=
            broadcast_element.multiplier_a[rank_a - j] * index[rank_bc - j];
      }
      while (j < rank_b) {
        j++;
        index_b +=
            broadcast_element.multiplier_b[rank_b - j] * index[rank_bc - j];
      }

      result_values[i] =
          values(operand_a)[index_a] + values(operand_b)[index_b];

      index[broadcast_element.rank - 1]++;
      int16_t index_dim = broadcast_element.rank - 1;
      while (index[index_dim] == broadcast_element.shape[index_dim] &&
             index_dim) {
        index[index_dim] = 0;
        if (index_dim) {
          index[--index_dim]++;
        }
      }
    }
    return result;
  }
}

void *laud_narray_dadd(const struct laud_narray *operand_a,
                       const struct laud_narray *operand_b,
                       uint64_t respect_index, struct laud_narray *pre_dx,
                       struct laud_narray *calc_result) {

  const struct laud_narray *operand =
      respect_index == 0 ? operand_a : operand_b;

  uint16_t rank_operand = rank(operand);

  uint64_t *shape_operand = shape(operand);

  struct laud_narray *derivatives =
      laud_narray(rank_operand, shape_operand, 0, NULL);

  float *derivative_values = values(derivatives);
  const float *const pre_dx_values = values(pre_dx);

  uint64_t length_operand = length(operand);
  uint64_t length_pre_dx = length(pre_dx);

  if (classOf(calc_result) == LaudNArrayBroadcast &&
      length_operand != length_pre_dx) {

    UbjectError.warn("bc!");

    uint16_t rank_pre_dx = rank(pre_dx);

    uint64_t index[rank_pre_dx];
    memset(index, 0, sizeof(index));

    uint64_t *multiplier =
        respect_index == 0
            ? ((struct laud_narray_bc *)calc_result)->multiplier_a
            : ((struct laud_narray_bc *)calc_result)->multiplier_b;

    uint64_t *shape_pre_dx = shape(pre_dx);

    for (uint64_t i = 0; i < length_pre_dx; i++) {

      uint64_t offset = 0;
      uint16_t j = 0;
      while (j < rank_operand) {
        j++;

        if (shape_operand[rank_operand - j] != 1) {

          offset += multiplier[rank_operand - j] * index[rank_pre_dx - j];
        }
      }

      derivative_values[offset] += pre_dx_values[i];

      index[rank_pre_dx - 1]++;
      int16_t index_dim = rank_pre_dx - 1;
      while (index[index_dim] == shape_pre_dx[index_dim]) {

        index[index_dim] = 0;
        if (index_dim) {
          index[--index_dim]++;
        }
      }
    }
  } else {

    for (uint64_t i = 0; i < length_operand; i++) {

      derivative_values[i] = pre_dx_values[i];
    }
  }

  return derivatives;
}

#endif

#ifdef LAUD_VAR_IMPLEMENTATION

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

#endif