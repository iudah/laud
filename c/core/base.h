#ifndef LAUD_BASE_H
#define LAUD_BASE_H

#ifdef _WIN32

#ifdef LAUDEXPORTS
#define LAUDAPI __declspec(dllexport)
#else
#define LAUDAPI __declspec(dllimport)
#endif
#define LAUDCALL __cdecl

#else

#define LAUDAPI
#define LAUDCALL

#endif

/**
 * @file base.h
 * @brief Header file containing functions for basic operations on Laud objects.
 */

#include <inttypes.h>
#include <mem_lk.h>
#include <stddef.h>
#include <stdint.h>

#ifndef LAUD_NUMBER_T
#define LAUD_NUMBER_T double
#endif

typedef LAUD_NUMBER_T number_t;

/**
 * Converts a Laud object to a human-readable string representation.
 *
 * @param laud_object The Laud object to convert to a string.
 * @param buffer The buffer to store the string representation.
 * @param buf_limit The maximum size of the buffer.
 * @return The buffer containing the string representation.
 */
LAUDAPI char *laud_to_string(const void *laud_object, char *buffer,
                             const uint64_t buf_limit);


/**
 * Returns a pointer to the shape of a Laud object (array of dimensions).
 *
 * @param laud_array The Laud array to get the shape of.
 * @return A pointer to an array of size_t elements representing the shape.
 */
LAUDAPI const uint64_t *laud_shape(void *laud_array)
    __attribute__((warn_unused_result));

/**
 * Returns the number of dimensions (rank) of a Laud object.
 *
 * @param laud_array The Laud array to get the rank of.
 * @return The number of dimensions (rank) of the Laud array.
 */
LAUDAPI uint16_t laud_rank(void *laud_array)
    __attribute__((warn_unused_result));

/**
 * @brief Evaluates a Laud object.
 *
 * @param laud_array The Laud object to evaluate.
 */
LAUDAPI void laud_evaluate(void *laud_array);

/**
 * @brief Differntiate a Laud object.
 *
 * @param laud_array The Laud object to differentiate.
 * @param pre_derivatives Derivatives to continue differentiation with.
 */
LAUDAPI void laud_differentiate(void *laud_array, const void *pre_derivatives);

/**
 * Performs element-wise addition between two Laud objects.
 *
 * @param operand The Laud object.
 * @param axis The axis or dimension to retain after reduction of Laud object.
 * @param callback The callback to achieve reduction of Laud object.
 * @param args Argument and extra data to be passed to callback
 * @return A new Laud object containing the result of the reduction. Laud object
 * has length = 1 when axis = 0, length = shape[axis - 1] when 1 <= axis <=
 * rank.
 */
LAUDAPI void *laud_reduce(void *operand, uint16_t axis,
                          number_t (*callback)(const number_t current_net,
                                               const number_t *const values,
                                               const void *args),
                          const void *args);

number_t laud_value_at_offset(void *operand, uint64_t offset);

#endif
