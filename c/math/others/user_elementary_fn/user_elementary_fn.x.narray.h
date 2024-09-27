#ifndef LAUD_USER_ELEMENTARY_FN_X_NARRAY_H
#define LAUD_USER_ELEMENTARY_FN_X_NARRAY_H

#include <Ubject.h>
#include <inttypes.h>
#include <mem_lk.h>
#include <stdint.h>
#include <stdlib.h>

// #define LAUD_NARRAY_BC_IMPLEMENTATION
#include "../../../core/narray.h"
#include "../../../core/narray.r.h"
#include "../../../core/narray.r.static.h"
#include "../../../core/narray_bc.r.h"
#include "../../../core/narray_bc.r.static.h"
#include "../../../math/others/user_elementary_fn/user_elementary_fn.r.h"

#include "user_elementary_fn.def.h"

struct laud_element_n_broadcast {
  uint64_t *multiplier;
  uint64_t **multipliers;
  uint64_t *shape;
  uint16_t rank;
};

static inline void
element_n_broadcast(struct laud_element_n_broadcast *broadcast,
                    const struct laud_narray **operands,
                    const uint64_t no_of_operands) {

  // const struct laud_narray *operand_a = broadcast->operand;
  // const struct laud_narray *operand_b = generator_b->operand;

  int16_t dims[no_of_operands];
  int16_t dim_bc = broadcast->rank = 1;
  for (uint64_t i = 0; i < no_of_operands; i++) {
    dims[i] = rank(operands[i]);
    dim_bc = broadcast->rank = dim_bc > dims[i] ? dim_bc : dims[i];
  }

  uint64_t *shape_bc = broadcast->shape =
      CALLOC(broadcast->rank, sizeof(uint64_t));
  uint64_t *multiplier_bc = broadcast->multiplier =
      CALLOC(broadcast->rank, sizeof(uint64_t));
  multiplier_bc[broadcast->rank - 1] = 1;

  uint64_t **multipliers = broadcast->multipliers =
      CALLOC(no_of_operands, sizeof(uint64_t *));
  for (uint64_t i = 0; i < no_of_operands; i++) {
    multipliers[i] = CALLOC(rank(operands[i]), sizeof(uint64_t));
  }

  int8_t discard_broadcast = 1;

  while (dim_bc) {
    dim_bc--;
    uint64_t dim_lenght = 1;
    for (uint64_t i = 0; i < no_of_operands; i++) {
      if (dims[i]) {
        dims[i]--;
        if (dim_lenght == 1) {
          dim_lenght = shape(operands[i])[dims[i]];
          if (i != 0) {
            // not the first operand but is first to register a value greater
            // than 1 then the previous operands are either having a 1 or have a
            // smaller number of dimensions so we can not discard the broadcast
            discard_broadcast = 0;
          }
        } else {
          if (dim_lenght != shape(operands[i])[dims[i]]) {
            if (shape(operands[i])[dims[i]] == 1) {
              // Preserve broadcast
              discard_broadcast = 0;
            } else {
              UbjectError.error("Operand %" PRIu64
                                " has differing length %" PRIu64 " compared "
                                "to %" PRIu64 ". Cannot broadcast.",
                                i, shape(operands[i])[dims[i]], dim_lenght);
            }
          } else {
            // matching operands
          }
        }

        if (dims[i] + 1 != rank(operands[i])) {
          multipliers[i][dims[i]] =
              multipliers[i][dims[i] + 1] * shape(operands[i])[dims[i] + 1];
        } else {
          multipliers[i][dims[i]] = 1;
        }
      }
    }

    shape_bc[dim_bc] = dim_lenght;
    if (dim_bc) {
      multiplier_bc[dim_bc - 1] = multiplier_bc[dim_bc] * dim_lenght;
    }
  }

  if (discard_broadcast) {
    FREE(shape_bc);

    for (uint64_t i = 0; i < no_of_operands; i++) {
      FREE(multipliers[i]);
    }
    FREE(multipliers);

    shape_bc = broadcast->shape = NULL;
    broadcast->multipliers = NULL;
    dim_bc = broadcast->rank = 0;
  }
}

_Thread_local struct {
  struct laud_narray *result;
  number_t **operands;

  uint64_t *multiplier;
  uint64_t **multipliers;
  uint64_t **shapes;

  uint64_t *lengths;
  uint16_t *rankd;

  uint64_t no_of_operands;
} _thread_local_broadcast_data_ = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0};

static void get_bc_value_fn(uint64_t offset, number_t *op_values) {

  uint64_t offsets[_thread_local_broadcast_data_.no_of_operands];
  memset(offsets, 0, sizeof(offsets));

  for (uint16_t i = 0; i < rank(_thread_local_broadcast_data_.result); i++) {
    uint64_t remainder = offset % _thread_local_broadcast_data_.multiplier[i];
    uint64_t index =
        (offset - remainder) / _thread_local_broadcast_data_.multiplier[i];
    offset = remainder;

    for (uint64_t j = 0; j < _thread_local_broadcast_data_.no_of_operands;
         j++) {

      if (i >= _thread_local_broadcast_data_.rankd[j]) {
        uint16_t r = i - _thread_local_broadcast_data_.rankd[j];
        offsets[j] +=
            _thread_local_broadcast_data_.shapes[j][r] > 1
                ? index * _thread_local_broadcast_data_.multipliers[j][r]
                : 0;
      }
    }
  }

  for (uint64_t j = 0; j < _thread_local_broadcast_data_.no_of_operands; j++) {
    op_values[j] = _thread_local_broadcast_data_.operands[j][offsets[j]];
  }
}

static void *narray_user_elementary_fn(laud_user_elementary_fn_t user_fn,
                                       uint64_t no_of_operands,
                                       const struct laud_narray **operands,
                                       void *args) {

  struct laud_element_n_broadcast broadcast_element = {
      .multipliers = NULL,
      .shape = NULL,
      .rank = 0,
  };

  element_n_broadcast(&broadcast_element, operands, no_of_operands);

  void *broadcast_shape___ = broadcast_element.shape;

  number_t *operands_values[no_of_operands];
  for (uint64_t i = 0; i < no_of_operands; i++) {
    operands_values[i] = values(operands[i]);
  }

  struct laud_narray *result = NULL;

  if (!broadcast_shape___) {
    uint16_t rank_ = rank(operands[0]);
    uint64_t *shape_ = shape(operands[0]);
    uint64_t lenght_ = length(operands[0]);

    result = laud_narray(rank_, shape_, 0, NULL);
    number_t *result_values = values(result);

    user_fn(result_values, lenght_, operands_values, NULL, args);

  } else {

    uint64_t **tmp = REALLOC(broadcast_element.multipliers,
                             (no_of_operands + 1) * sizeof(uint64_t *));
    if (!tmp)
      UbjectError.error("insufficient memory");
    tmp[no_of_operands] = NULL;
    broadcast_element.multipliers = (uint64_t **)tmp;
    result =
        laud_narray_bc(broadcast_element.rank, broadcast_element.shape, 0, NULL,
                       (const uint64_t *)broadcast_element.multipliers, NULL);
    number_t *result_values = values(result);

    uint64_t length_bc = length(result);

    _thread_local_broadcast_data_.result = result;
    _thread_local_broadcast_data_.multipliers = broadcast_element.multipliers;
    _thread_local_broadcast_data_.multiplier = broadcast_element.multiplier;
    _thread_local_broadcast_data_.no_of_operands = no_of_operands;
    _thread_local_broadcast_data_.operands = operands_values;

    uint64_t lengths[no_of_operands];
    uint16_t rankd[no_of_operands];
    uint64_t *shapes[no_of_operands];
    for (uint64_t i = 0; i < no_of_operands; i++) {
      lengths[i] = length(operands[i]);
      rankd[i] = broadcast_element.rank - rank(operands[i]);
      shapes[i] = shape(operands[i]);
    }

    _thread_local_broadcast_data_.lengths = lengths;
    _thread_local_broadcast_data_.rankd = rankd;
    _thread_local_broadcast_data_.shapes = shapes;

    user_fn(result_values, length_bc, operands_values, get_bc_value_fn, args);
  }
  if (broadcast_element.shape)
    FREE(broadcast_element.shape);
  if (broadcast_element.multiplier)
    FREE(broadcast_element.multiplier);
  return result;
}

void *laud_narray_duser_elementary_fn(const struct laud_narray *operand_a,
                                      const struct laud_narray *operand_b,
                                      uint64_t respect_index,
                                      const struct laud_narray *pre_dx,
                                      struct laud_narray *calc_result) {
  UbjectError.error("Can't differentiate user function");
  const struct laud_narray *operand =
      respect_index == 0 ? operand_a : operand_b;

  uint16_t rank_operand = rank(operand);

  uint64_t *shape_operand = shape(operand);

  struct laud_narray *derivatives =
      laud_narray(rank_operand, shape_operand, 0, NULL);

  number_t *derivative_values = values(derivatives);
  const number_t *const pre_dx_values = values(pre_dx);

  uint64_t length_operand = length(operand);
  uint64_t length_pre_dx = length(pre_dx);

  if (classOf(calc_result) == LaudNArrayBroadcast &&
      length_operand != length_pre_dx) {

    uint16_t rank_pre_dx = rank(pre_dx);

    uint64_t index[rank_pre_dx];
    memset(index, 0, sizeof(index));

    uint64_t *multiplier =
        respect_index == 0 ? multiplier_a((struct laud_narray_bc *)calc_result)
                           : multiplier_b((struct laud_narray_bc *)calc_result);

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
