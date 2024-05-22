#ifndef NARRAY_BC_R_H
#define NARRAY_BC_R_H

/**
 * @file narray_bc.r.h
 * @brief Header file defining the structure for N-dimensional arrays with
 * broadcasted operands in Laud.
 */

#include <stddef.h>

#include "./narray.r.h"

/**
 * @brief Represents a broadcasted N-dimensional array in Laud.
 */
struct laud_narray_bc {

#ifndef LAUD_NARRAY_BC_IMPLEMENTATION
  /* Private members to simulate encapsulation */
  char ___[sizeof(struct laud_narray_bc_private {

#endif
    struct laud_narray _;   /**< Parent structure for n-dimensional objects */
    uint64_t *multiplier_a; /**< Multiplier to take care of differentiation of
                               the bc'd array */
    uint64_t *multiplier_b; /**< Multiplier to take care of differentiation of
                               the bc'd array */

#ifndef LAUD_NARRAY_BC_IMPLEMENTATION
  })];
#endif
};

#define LAUD_NARRAY_BC_PRIORITY (LAUD_NARRAY_PRIORITY + 1)

extern const void *LaudNArrayBroadcast; /**< Pointer to LaudNArray */

void *laud_narray_bc(const uint16_t rank, const uint64_t *const shape,
                     const uint64_t length, const float *const data,
                     const uint64_t *const multiplier_a,
                     const uint64_t *const multiplier_b)
    __attribute__((malloc, warn_unused_result));

#ifdef LAUD_NARRAY_BC_IMPLEMENTATION_REQUIRED

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
      malloc(broadcast->rank * sizeof(uint64_t));

  uint64_t *multiplier_a = broadcast->multiplier_a =
      malloc(rank(operand_a) * sizeof(uint64_t));
  uint64_t *multiplier_b = broadcast->multiplier_b =
      malloc(rank(operand_b) * sizeof(uint64_t));

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
          free(shape_bc);
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
    free(shape_bc);
    free(multiplier_a);
    free(multiplier_b);
    shape_bc = broadcast->shape = broadcast->multiplier_a =
        broadcast->multiplier_b = NULL;
    dim_bc = broadcast->rank = 0;
  }
}

#endif

#endif
