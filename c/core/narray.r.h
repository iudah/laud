#ifndef NARRAY_R_H
#define NARRAY_R_H

/**
 * @file narray_r.h
 * @brief Header file defining the structure and functions for N-dimensional
 * arrays in Laud.
 */

#include <stddef.h>

#include "./base.r.h"

/**
 * @brief Represents an N-dimensional array in Laud.
 */
struct laud_narray {

#ifndef LAUD_NARRAY_IMPLEMENTATION
  /* Private members to simulate encapsulation */
  char ___[sizeof(struct laud_narray_private {

#endif
    struct laud_base _; /**< Base structure for Laud objects */
    number_t *values;   /**< Pointer to the array of values */
    uint64_t *shape;    /**< Pointer to the array specifying the dimensions */
    uint64_t length;    /**< Total number of elements in the array */
    uint16_t rank;      /**< Number of dimensions (rank) of the array */
    struct graph_node
        *computation_node; /**< Pointer to computation graph node */

#ifndef LAUD_NARRAY_IMPLEMENTATION
  })];
#endif
};

#ifdef LAUD_NARRAY_IMPLEMENTATION
/**
 * @brief Returns the rank of the N-dimensional array.
 *
 * @param narray The N-dimensional array.
 * @return The rank of the array.
 */
static inline uint16_t rank(const struct laud_narray *narray) {
  return narray->rank;
}

/**
 * @brief Returns the shape (dimensions) of the N-dimensional array.
 *
 * @param narray The N-dimensional array.
 * @return A pointer to the array of dimensions.
 */
static inline uint64_t *shape(const struct laud_narray *narray) {
  return narray->shape;
}

/**
 * @brief Returns the total number of elements in the N-dimensional array.
 *
 * @param narray The N-dimensional array.
 * @return The total number of elements in the array.
 */
static inline uint64_t length(const struct laud_narray *narray) {
  return narray->length;
}

/**
 * @brief Returns a pointer to the array of values of the N-dimensional array.
 *
 * @param narray The N-dimensional array.
 * @return A pointer to the array of values.
 */
static inline number_t *values(const struct laud_narray *narray) {
  return narray->values;
}

#endif

extern const void *LaudNArray; /**< Pointer to LaudNArray */

#define LAUD_NARRAY_PRIORITY (LAUD_BASE_PRIORITY + 1)

struct laud_narray *
derivative_add(struct laud_narray *var, struct laud_narray *sum_of_derivatives,
               const struct laud_narray *derivative_wrt_var);

#endif
