/**
 * @file Var.h
 *
 * @brief Declarations for the LaudVar data type and associated functions.
 */

#ifndef VAR_H
#define VAR_H

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

#include <stddef.h>

#include "Placeholder.h"

/**
 * @brief External object declaration for LaudVar.
 */
extern const void *LaudVar;

/**
 * @brief Create a LaudVar object.
 *
 * @param rank The rank of the LaudVar.
 * @param shape An array specifying the shape of the LaudVar.
 * @param length The length of the data array.
 * @param data The data array for LaudVar. This array is copied.
 * @return A pointer to the newly created LaudVar object.
 */
LAUDAPI void *laud_var(size_t const rank, size_t const *const shape,
                       size_t const length, float const *const data)
    __attribute__((malloc));

/**
 * @brief Delete a LaudVar object and release allocated resources.
 *
 * @param self A pointer to the LaudVar object to be deleted.
 */
LAUDAPI void laud_delete(void *self);

/**
 * @brief Increments the reference count of a LaudVar object.
 *
 * This function increments the reference count of the provided LaudVar object.
 * The reference count is used to track the number of references to the object,
 * preventing premature destruction when multiple references exist.
 *
 * @param self A pointer to the LaudVar object for which the reference count
 * will be incremented.
 * @return A pointer to the input LaudVar object.
 */
LAUDAPI void *laud_copy(const void *self);

/**
 * @brief Set the value of a specific index in a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @param index The index to set the value.
 * @param value The value to set.
 * @return The older value at the specified index.
 */
LAUDAPI float laud_set_value(void *self, const size_t index, const float value);

/**
 * @brief Get the value at a specific index in a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @param index The index to retrieve the value.
 * @return The value at the specified index.
 */
LAUDAPI float laud_get_value(const void *self, const size_t index);

/**
 * @brief Check if a LaudVar object holds a float value.
 *
 * @param self A pointer to the LaudVar object.
 * @return 1 if the LaudVar object holds a float value, 0 otherwise.
 */
LAUDAPI char laud_is_float(const void *self);

/**
 * @brief Get the length of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @return The length of the LaudVar object.
 */
LAUDAPI size_t laud_length(const void *self);

/**
 * @brief Get the values array of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @return A pointer to the values array of the LaudVar object.
 */
LAUDAPI const float *laud_values(const void *self);

/**
 * @brief Get the rank of a LaudVar object.
 *
 * @param a A pointer to the LaudVar object.
 * @return The rank of the LaudVar object.
 */
LAUDAPI size_t laud_rank(const void *a);
/**
 * @brief Get the shape array of a LaudVar object.
 *
 * @param a A pointer to the LaudVar object.
 * @return A pointer to the shape array of the LaudVar object.
 */
LAUDAPI const size_t *laud_shape(const void *a);

/**
 * @brief Evaluate a LaudVar object and return a pointer to the result.
 *
 * @param self A pointer to the LaudVar object to be evaluated.
 * @return A pointer to the result of the evaluation.
 */
LAUDAPI void *laud_evaluate(void *self);

/**
 * @brief Compute the derivative of a LaudVar object with respect to itself.
 *
 * @param self A pointer to the LaudVar object.
 * @param derivative A pointer to the derivative object.
 * @param ddx A map of pointers to store the derivatives.
 * @return The number of derivatives computed.
 */
LAUDAPI int laud_differentiate(const void *self, void *derivative, void *ddx);

/**
 * @brief Create a map to store derivatives.
 *
 * @return A pointer to the created derivative map.
 */
LAUDAPI void *laud_create_derivative_map();

/**
 * @brief Delete a derivative map and release allocated resources.
 *
 * @param map A pointer to the derivative map to be deleted.
 */
LAUDAPI void laud_delete_derivative_map(void *map);

/**
 * @brief Start iteration over a derivative map and return the initial position.
 *
 * @param map A pointer to the derivative map.
 * @return The initial position for iteration.
 */
LAUDAPI size_t laud_start_derivative_map_iteration(void *map);

/**
 * @brief Yield the next value in a derivative map iteration.
 *
 * @param map A pointer to the derivative map.
 * @return A pointer to the next value in the iteration.
 */
LAUDAPI void **laud_yield_derivative_map_value(void *map);

/**
 * @brief Set the continuity of a LaudVar object (continuous or discontinuous).
 *
 * @param self A pointer to the LaudVar object.
 * @param continuous Flag indicating continuity (1 for continuous, 0 for
 * discontinuous).
 */
LAUDAPI void laud_set_continuity(void *self, char continuous);

/**
 * @brief Check if a LaudVar object is continuous.
 *
 * @param self A pointer to the LaudVar object.
 * @return 1 if the LaudVar object is continuous, 0 if it is discontinuous.
 */
LAUDAPI char laud_is_continuous(const void *self);

/**
 * @brief Check if a LaudVar object is constant.
 *
 * @param self A pointer to the LaudVar object.
 * @return 1 if the LaudVar object is constant, 0 if it is not constant.
 */
LAUDAPI char laud_is_constant(const void *self);

/**
 * @brief External object declaration for LaudAdd.
 */
extern const void *LaudAdd;

/**
 * @brief External object declaration for LaudMinus.
 */
extern const void *LaudMinus;

/**
 * @brief External object declaration for LaudProduct.
 */
extern const void *LaudProduct;

/**
 * @brief External object declaration for LaudQuotient.
 */
extern const void *LaudQuotient;

/**
 * @brief External object declaration for LaudMatrixDot.
 */
extern const void *LaudMatrixDot;

/**
 * @brief Perform addition between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * addition.
 */
LAUDAPI void *laud_add(const void *a, const void *b);

/**
 * @brief Perform subtraction between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * subtraction.
 */
LAUDAPI void *laud_minus(const void *a, const void *b);

/**
 * @brief Perform multiplication between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * multiplication.
 */
LAUDAPI void *laud_product(const void *a, const void *b);

/**
 * @brief Perform division between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * division.
 */
LAUDAPI void *laud_quotient(const void *a, const void *b);

/**
 * @brief Perform matrix dot product between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * matrix dot product.
 */
LAUDAPI void *laud_matrix_dot(const void *a, const void *b);

/**
 * @brief Get a string representation of a LaudVar object.
 *
 * @param a A pointer to the LaudVar object.
 * @param buffer A buffer to store the string representation.
 * @param limit The maximum number of characters to write to the buffer.
 * @return A pointer to the buffer containing the string representation.
 */
LAUDAPI char *laud_as_string(void *a, char *buffer, size_t limit);

/**
 * @brief Change the dependency of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @param dependency A pointer to the new dependency.
 * @param index The index to change the dependency.
 * @return A pointer to the modified LaudVar object.
 */
LAUDAPI void *laud_change_dependency(void *self, void *dependency, size_t index)
    __attribute__((warn_unused_result));

/**
 * @brief Slice a LaudVar object based on the specified format.
 *
 * @param self A pointer to the LaudVar object.
 * @param slice_fmt The format string for slicing.
 * @param ... Additional arguments based on the slice format.
 * @return A pointer to the sliced LaudVar object.
 */
LAUDAPI void *laud_slice(const void *self, const char *slice_fmt, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @brief Shuffle the dimensions of a LaudVar object.
 *
 * @param x A pointer to the LaudVar object to be shuffled.
 * @param n_dim The number of dimensions to shuffle.
 * @param dims An array specifying the dimensions to shuffle.
 * @return A pointer to the shuffled LaudVar object.
 */
LAUDAPI void *laud_shuffle(const void *x, size_t n_dim, size_t *dims);

/**
 * @brief Shuffle the elements of a LaudArray in the same pattern as another
 * LaudArray.
 *
 * @param self      The LaudArray to be shuffled.
 * @param shuffled  The LaudArray that provides the shuffling pattern.
 *
 * @return          A new LaudArray with elements shuffled accordingly.
 */
LAUDAPI void *laud_shuffle_like(const void *self, const void *shuffled);

/**
 * @brief Generate slices of a LaudVar object based on the specified format.
 *
 * @param x A pointer to the LaudVar object.
 * @param fmt The format string for generating slices.
 * @param ... Additional arguments based on the format.
 * @return A pointer to the generated slices of the LaudVar object.
 */
LAUDAPI const void *laud_generate_slices(const void *x, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @brief Yield values from a generator LaudVar object.
 *
 * @param generator A pointer to the generator LaudVar object.
 * @return A pointer to the yielded values.
 */
LAUDAPI const void *laud_yield(const void *generator);

/**
 * @brief Create a LaudVar object from a user-defined function.
 *
 * @param generator A function pointer to the generator function.
 * @param rank The rank of the LaudVar to be generated.
 * @param shape An array specifying the shape of the LaudVar.
 * @param usr_args Additional user-defined arguments for the generator function.
 * @return A pointer to the generated LaudVar object.
 */
LAUDAPI void *laud_from_function(
    float (*generator)(const size_t rank, const size_t *const shape,
                       const size_t offset, const void *const usr_args),
    const size_t rank, size_t *shape, const void *usr_args);

/**
 * @brief Create a LaudVar object from a text file.
 *
 * @param file_path The path to the text file.
 * @param delim The delimiter used in the text file.
 * @return A pointer to the LaudVar object created from the text file.
 */
LAUDAPI void *laud_from_text(const char *file_path, const char *delim);

#endif
