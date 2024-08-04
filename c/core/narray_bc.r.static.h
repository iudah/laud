#ifndef NARRAY_BC_R_STATIC_H
#define NARRAY_BC_R_STATIC_H

#include <Ubject.h>
#include <mem_lk.h>
#include <stdint.h>
#include <stdlib.h>

#define LAUD_NARRAY_BC_IMPLEMENTATION
#include "narray.r.static.h"
#include "narray_bc.r.h"

static inline uint64_t *multiplier_a(struct laud_narray_bc *broadcast) {
  return broadcast->multiplier_a;
}

static inline uint64_t *multiplier_b(struct laud_narray_bc *broadcast) {
  return broadcast->multiplier_b;
}

struct laud_element_broadcast {
  uint64_t *multiplier_a;
  uint64_t *multiplier_b;
  uint64_t *shape;
  uint16_t rank;
};

static inline void element_broadcast(struct laud_element_broadcast *broadcast,
                                     const struct laud_narray *operand_a,
                                     const struct laud_narray *operand_b) {

  // const struct laud_narray *operand_a = broadcast->operand;
  // const struct laud_narray *operand_b = generator_b->operand;

  int16_t dim_a = rank(operand_a);
  int16_t dim_b = rank(operand_b);
  int16_t dim_bc = broadcast->rank = dim_a > dim_b ? dim_a : dim_b;

  uint64_t *shape_bc = broadcast->shape =
      CALLOC(broadcast->rank, sizeof(uint64_t));

  uint64_t *multiplier_a = broadcast->multiplier_a =
      CALLOC(rank(operand_a), sizeof(uint64_t));
  uint64_t *multiplier_b = broadcast->multiplier_b =
      CALLOC(rank(operand_b), sizeof(uint64_t));

  int8_t discard_broadcast = 1;

  while (dim_bc) {
    dim_a--;
    dim_b--;
    dim_bc--;

    if (dim_a >= 0 && dim_b >= 0) {
      multiplier_a[dim_a] =
          dim_a == (rank(operand_a) - 1)
              ? 1
              : (multiplier_a[dim_a + 1] * shape(operand_a)[dim_a + 1]);
      multiplier_b[dim_b] =
          dim_b == (rank(operand_b) - 1)
              ? 1
              : (multiplier_b[dim_b + 1] * shape(operand_b)[dim_b + 1]);

      if (shape(operand_a)[dim_a] != shape(operand_b)[dim_b]) {
        if (shape(operand_a)[dim_a] == 1) {
          shape_bc[dim_bc] = shape(operand_b)[dim_b];
        } else if (shape(operand_b)[dim_b] == 1) {
          shape_bc[dim_bc] = shape(operand_a)[dim_a];
        } else {
          FREE(shape_bc);
          UbjectError.error(
              "operands are not compatible for elementary operation");
        }
        discard_broadcast = 0;
      } else {
        shape_bc[dim_bc] = shape(operand_b)[dim_b];
      }
    } else if (dim_a >= 0 && dim_b < 0) {
      multiplier_a[dim_a] =
          dim_a == (rank(operand_a) - 1)
              ? 1
              : (multiplier_a[dim_a + 1] * shape(operand_a)[dim_a + 1]);
      shape_bc[dim_bc] = shape(operand_a)[dim_a];
    } else if (dim_a < 0 && dim_b >= 0) {
      multiplier_b[dim_b] =
          dim_b == (rank(operand_b) - 1)
              ? 1
              : (multiplier_b[dim_b + 1] * shape(operand_b)[dim_b + 1]);
      shape_bc[dim_bc] = shape(operand_b)[dim_b];
    }
  }
  if (dim_a == dim_b && discard_broadcast) {
    FREE(shape_bc);
    FREE(multiplier_a);
    FREE(multiplier_b);
    shape_bc = broadcast->shape = broadcast->multiplier_a =
        broadcast->multiplier_b = NULL;
    dim_bc = broadcast->rank = 0;
  }
}

#endif
