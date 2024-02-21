#ifndef BASE_H
#define BASE_H

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

#include <stddef.h>

/**
 * Performs element-wise addition between two Laud objects.
 *
 * @param operand_a The first Laud object.
 * @param operand_b The second Laud object.
 * @return A new Laud object containing the result of the addition.
 */
LAUDAPI void *laud_add(void *operand_a, void *operand_b);

/**
 * Computes the matrix dot product of two Laud objects.
 *
 * @param operand_a The first Laud object.
 * @param operand_b The second Laud object.
 * @return A new Laud object containing the result of the matrix dot product.
 */
LAUDAPI void *laud_matrix_dot(void *operand_a, void *operand_b);

/**
 * Converts a Laud object to a human-readable string representation.
 *
 * @param laud_object The Laud object to convert to a string.
 * @param buffer The buffer to store the string representation.
 * @param buf_limit The maximum size of the buffer.
 * @return The buffer containing the string representation.
 */
LAUDAPI char *laud_to_string(void *laud_object, char *buffer, size_t buf_limit);

/**
 * Extracts a sub-array from a Laud object using a format string.
 *
 * @param self The Laud object to slice.
 * @param slice_format The format string specifying the slice dimensions.
 * @return A new Laud object containing the extracted sub-array.
 */
LAUDAPI void *laud_slice(const void *self, const char *slice_format, ...)
    __attribute__((format(printf, 2, 3), warn_unused_result));

/**
 * Returns a pointer to the shape of a Laud object (array of dimensions).
 *
 * @param laud_array The Laud array to get the shape of.
 * @return A pointer to an array of size_t elements representing the shape.
 */
LAUDAPI const size_t *laud_shape(void *laud_array)
    __attribute__((warn_unused_result));

/**
 * Returns the number of dimensions (rank) of a Laud object.
 *
 * @param laud_array The Laud array to get the rank of.
 * @return The number of dimensions (rank) of the Laud array.
 */
LAUDAPI size_t laud_rank(void *laud_array) __attribute__((warn_unused_result));

/**
 * @brief Evaluates a Laud object.
 *
 * @param laud_array The Laud object to evaluate.
 */
LAUDAPI void laud_evaluate(void *laud_array);

#endif
