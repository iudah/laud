#ifndef BASE_R_H
#define BASE_R_H

#include <TypeClass.r.h>
#include <Ubject.r.h>

// Forward declaration of struct laud_base
struct laud_base;

/**
 * @brief Provides the base functionality for Laud objects.
 *
 * This class defines a set of fundamental operations that can be performed
 * on Laud objects, such as arithmetic, string conversion, slicing, and
 * mathematical functions.
 */
struct laud_base_class {

  const struct TypeClass _; // Inherits from TypeClass

  /**
   * @brief Adds two Laud objects together.
   *
   * @param operand_a The first operand.
   * @param operand_b The second operand.
   * @param null Unused parameter (reserved for future use).
   * @return A new Laud object representing the sum of the operands.
   */
  void *(*add)(void *operand_a, void *operand_b, void *null);

  /**
   * @brief Performs matrix multiplication on two Laud objects.
   *
   * @param operand_a The first operand (left matrix).
   * @param operand_b The second operand (right matrix).
   * @param null Unused parameter (reserved for future use).
   * @return A new Laud object representing the product of the matrices.
   */
  void *(*matrix_dot)(void *operand_a, void *operand_b, void *null);

  /**
   * @brief Converts a Laud object to a string representation.
   *
   * This function converts a Laud object to a string representation and places
   * the result in the provided buffer.
   *
   * @param operand The Laud object to convert.
   * @param buffer A character array to store the string representation.
   * @param buffer_limit The maximum size of the `buffer` in bytes.
   * @return Pointer to the `buffer` containing the string representation, or
   * `NULL` on error.
   */
  void *(*to_string)(const void *operand, char *buffer,
                     const size_t buffer_limit);

  /**
   * @brief Slices a Laud object based on a given format.
   *
   * @param operand The Laud object to slice.
   * @param slice_format The format string describing the slice.
   * @param null Unused parameter (reserved for future use).
   * @return A new Laud object representing the sliced portion.
   */
  void *(*slice)(const void *operand, const char *slice_format, void *null);

  /**
   * @brief Applies the rectified linear unit function to a Laud object.
   *
   * @param operand The Laud object to apply the function to.
   * @param null Unused parameter (reserved for future use).
   * @return A new Laud object resulting from applying the function.
   */
  void *(*relu)(const void *operand, void *null);

  /**
   * @brief Applies the sigmoid function to a Laud object.
   *
   * @param operand The Laud object to apply the function to.
   * @param null Unused parameter (reserved for future use).
   * @return A new Laud object resulting from applying the function.
   */
  void *(*sigmoid)(const void *operand, void *null);

  /**
   * @brief Returns the shape of a Laud object.
   *
   * @param operand The Laud object to retrieve the shape from.
   * @return An array containing the dimensions of the Laud object.
   */
  size_t *(*shape)(const void *operand);

  /**
   * @brief Returns the rank of a Laud object.
   *
   * @param operand The Laud object to determine the rank of.
   * @return The rank of the Laud object.
   */
  size_t (*rank)(const void *operand);

  /**
   * @brief Evaluates a Laud object.
   *
   * @param operand The Laud object to evaluate.
   * @param null Unused parameter (reserved for future use).
   */
  void (*evaluate)(const void *operand, void *null);
};

/**
 * @brief Represents a Laud object.
 *
 * This struct serves as the base for all Laud objects, inheriting
 * functionality from the Ubject class.
 */
struct laud_base {

  const struct Ubject _;
};

#define LAUD_BASE_PRIORITY (UBJECT_PRIORITY + 3)

extern const void *LaudBase;      /**< Pointer to LaudBase */
extern const void *LaudBaseClass; /**< Pointer to LaudBaseClass */

#endif
