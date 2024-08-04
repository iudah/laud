#ifndef NARRAY_R_STATIC_H
#define NARRAY_R_STATIC_H

#include <stddef.h>

#define LAUD_NARRAY_IMPLEMENTATION
#include "narray.r.h"

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
