#ifndef LAUD_NARRAY_R_STATIC_H
#define LAUD_NARRAY_R_STATIC_H

#include "BaseObject.h"
#include "Ubject.r.h"
#include "mem_lk.h"
#include <stddef.h>
#include <stdint.h>

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

static inline void set_meta_data(struct laud_narray *output, void *meta) {
  output->meta_data = meta;
}

static inline void *get_meta_data(const struct laud_narray *output) {
  return output->meta_data;
}

#endif
