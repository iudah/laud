#ifndef NARRAY_H
#define NARRAY_H

/**
 * @file narray.h
 * @brief Header file containing functions for working with N-dimensional arrays
 * in Laud.
 */

#include <stddef.h>

#include "base.h"

/**
 * @brief Creates a new Laud N-dimensional array.
 *
 * @param rank The number of dimensions of the array.
 * @param shape An array specifying the size of each dimension.
 * @param length The total number of elements in the array.
 * @param data An array containing the values to populate the array with.
 * @return A pointer to the newly created Laud N-dimensional array.
 */
LAUDAPI void *laud_narray(const uint16_t rank, const uint64_t *const shape,
                          const uint64_t prefill_data_length,
                          const float *const prefill_float_array)
    __attribute__((malloc, warn_unused_result));

/**
 * @brief Creates a Laud N-dimensional array from a generator function.
 *
 * @param generator The generator function to populate the array.
 * @param rank The number of dimensions of the array.
 * @param shape An array specifying the size of each dimension.
 * @param usr_args Additional user-defined arguments for the generator function.
 * @return A pointer to the newly created Laud N-dimensional array.
 */
LAUDAPI void *laud_from_function(
    float (*generator)(const uint16_t rank, const uint64_t *const shape,
                       const uint64_t offset, const void *const usr_args),
    const uint16_t rank, uint64_t *shape, const void *usr_args)
    __attribute__((malloc, warn_unused_result));

/**
 * @brief Creates a Laud N-dimensional array from text data.
 *
 * @param file_path The path to the file containing the data.
 * @param delim The delimiter used in the text file.
 * @return A pointer to the newly created Laud N-dimensional array.
 */
LAUDAPI void *laud_from_text(const char *file_path, const char *delim)
    __attribute__((warn_unused_result));

/**
 * @brief Yields the next slice from a slice generator.
 *
 * @param generator The slice generator.
 * @return A pointer to the next slice.
 */
LAUDAPI const void *laud_yield_slice(const void *generator)
    __attribute__((warn_unused_result));

/**
 * @brief Checks if an object is a Laud N-dimensional array.
 *
 * @param laud_object The object to check.
 * @return 1 if the object is a Laud N-dimensional array, otherwise 0.
 */
LAUDAPI char is_laud_narray(const void *nodelaud_object);

#endif
