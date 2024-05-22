#ifndef BINARY_CROSS_ENTROPY_XTERN_H
#define BINARY_CROSS_ENTROPY_XTERN_H

#ifdef LAUD_NARRAY_IMPLEMENTATION

static void *narray_binary_cross_entropy(const struct laud_narray *operand_a,
                                         const struct laud_narray *operand_b) {
  if (rank(operand_a) > 2 || rank(operand_b) > 2) {
    UbjectError.error("is log loss not limited to 2-d arrays? please contact "
                      "me on olaolujude@gmail.com to explain it. thanks.");
  }

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

    uint64_t one = 1;

    struct laud_narray *result = laud_narray(one, &one, 0, NULL);
    float *result_values = values(result);

    for (uint64_t i = 0; i < lenght_; i++) {

      float y = values(operand_a)[i];
      float p = values(operand_b)[i];
      // Todo: use the following formula for categorical cross entropy:
      //  result_values[i] += (values(operand_a)[i]*
      //  log(values(operand_b)[i]));
      result_values[0] += -y * log(p) - (1 - y) * log(1 - p);
    }
    result_values[0] /= shape_[0];

    // free(shape_);

    return result;
  } else {
    uint16_t rank_a = rank(operand_a);
    uint16_t rank_b = rank(operand_b);
    uint16_t rank_bc = broadcast_element.rank;

    uint64_t *shape_a = shape(operand_a);
    uint64_t *shape_b = shape(operand_b);

    uint64_t one = 1;

    struct laud_narray *result =
        laud_narray_bc(one, &one, 0, NULL, broadcast_element.multiplier_a,
                       broadcast_element.multiplier_b);
    float *result_values = values(result);

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

      float y = values(operand_a)[index_a];
      float p = values(operand_b)[index_b];

      result_values[0] = result_values[0] += -y * log(p) - (1 - y) * log(1 - p);

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
    result_values[0] /= broadcast_element.shape[0];
    return result;
  }
}
void *laud_narray_dbinary_cross_entropy(const struct laud_narray *operand_a,
                                        const struct laud_narray *operand_b,
                                        uint64_t respect_index,
                                        struct laud_narray *pre_dx,
                                        struct laud_narray *calc_result) {

  const struct laud_narray *operand =
      respect_index == 0 ? operand_a : operand_b;

  uint16_t rank_operand = rank(operand);

  uint64_t *shape_operand = shape(operand);

  struct laud_narray *derivatives =
      laud_narray(rank_operand, shape_operand, 0, NULL);

  float *derivative_values = values(derivatives);
  const float *const pre_dx_values = pre_dx ? values(pre_dx) : NULL;

  uint64_t length_operand = length(operand);

  if (classOf(calc_result) == LaudNArrayBroadcast) {
    UbjectError.error("differentiation of broadcasted bce not implemented");

    UbjectError.warn("bc!");

    uint64_t *multiplier_a_bc_rank =
        ((struct laud_narray_bc *)calc_result)->multiplier_a;
    uint64_t *multiplier_b_bc_shape =
        ((struct laud_narray_bc *)calc_result)->multiplier_b;

    uint64_t *multiplier =
        respect_index == 0 ? multiplier_a_bc_rank : multiplier_b_bc_shape;

    uint16_t rank_a = rank(operand_a);
    uint16_t rank_b = rank(operand_b);

    uint16_t rank_bc = (uint16_t)multiplier_a_bc_rank[rank_a];
    uint64_t length_broadcast = multiplier_a_bc_rank[rank_a + 1];
    uint64_t *shape_bc = multiplier_b_bc_shape + rank_b;

    uint64_t index[rank_bc];
    memset(index, 0, sizeof(index));

    for (uint64_t i = 0; i < length_broadcast; i++) {

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
          (pre_dx ? pre_dx_values[0] : 1) * (respect_index == 1
                                                 ? (-y / p + (1 - y) / (1 - p))
                                                 : log((1 - p) / p));

      index[rank_bc - 1]++;
      int16_t index_dim = rank_bc - 1;
      while (index[index_dim] == shape_bc[index_dim]) {

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
      derivative_values[i] = (pre_dx ? pre_dx_values[0] : 1) *
                             (respect_index == 1 ? (-y / p + (1 - y) / (1 - p))
                                                 : log((1 - p) / p));
    }
  }

  return derivatives;
}

#endif

#ifdef LAUD_VAR_IMPLEMENTATION

#include "../../../math/nn/binary_cross_entropy/binary_cross_entropy.r.h"

static void *var_binary_cross_entropy(const struct laud_var *operand_a,
                                      const struct laud_var *operand_b) {
  struct laud_var *bce =
      init(LaudBinaryCrossEntropy, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    laud_evaluate_var_node(bce);
  }

  return bce;
}

#endif

#endif
