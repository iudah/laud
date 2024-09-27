#ifndef LAUD_NARRAY_BC_R_H
#define LAUD_NARRAY_BC_R_H

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
                     const uint64_t length, const number_t *const data,
                     const uint64_t *const multiplier_a,
                     const uint64_t *const multiplier_b)
    __attribute__((malloc, warn_unused_result));

#endif
