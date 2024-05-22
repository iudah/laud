#ifndef MSE_XTERN_H
#define MSE_XTERN_H

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_mse(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b) {
  UbjectError.error("sorry cannot compute mse of your array atm");
}

void *laud_narray_dmse(const struct laud_narray *operand_a,
                       const struct laud_narray *operand_b,
                       uint64_t respect_index, struct laud_narray *pre_dx,
                       struct laud_narray *calc_result) {
  // todo: implement dmse function
  abort();

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

    uint64_t *multiplier =
        respect_index == 0
            ? ((struct laud_narray_bc *)calc_result)->multiplier_a
            : ((struct laud_narray_bc *)calc_result)->multiplier_b;

    uint16_t rank_bc = rank(pre_dx);

    uint64_t index[rank_bc];
    memset(index, 0, sizeof(index));

    uint64_t *shape_pre_dx = shape(pre_dx);

    for (uint64_t i = 0; i < length_pre_dx; i++) {

      uint64_t offset = 0;
      uint16_t j = 0;
      while (j < rank_operand) {
        j++;

        if (shape_operand[rank_operand - j] != 1) {

          offset += multiplier[rank_operand - j] * index[rank_bc - j];
        }
      }

      float y = values(operand_a)[offset];
      float p = values(operand_b)[offset];
      derivative_values[offset] =
          pre_dx_values[0] * (respect_index == 1 ? (-y / p + (1 - y) / (1 - p))
                                                 : log((1 - p) / p));

      index[rank_bc - 1]++;
      int16_t index_dim = rank_bc - 1;
      while (index[index_dim] == shape_pre_dx[index_dim]) {

        index[index_dim] = 0;
        if (index_dim) {
          index[--index_dim]++;
        }
      }
    }
  } else {

    for (uint64_t i = 0; i < length_operand; i++) {

      float y = values(operand_a)[i];
      float p = values(operand_b)[i];
      derivative_values[i] =
          pre_dx_values[0] * (respect_index == 1 ? (-y / p + (1 - y) / (1 - p))
                                                 : log((1 - p) / p));
    }
  }

  return derivatives;
}

#endif

#ifdef LAUD_VAR_IMPLEMENTATION

#include "../../../math/nn/mse/mse.r.h"

static void *var_mse(const struct laud_var *operand_a,
                     const struct laud_var *operand_b) {
  struct laud_var *mse = init(LaudMSE, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    laud_evaluate_var_node(mse);
  }

  return mse;
}

#endif

#endif