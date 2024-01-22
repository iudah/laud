/**
 * @file Var_r.h
 *
 * This file defines the structure and declarations related to LaudVar objects.
 */
#ifndef VAR_R_H
#define VAR_R_H

#include "Placeholder.r.h"
#include "ds/HashMap.h"

#define LAUD_VAR_PRIORITY (UBJECT_PRIORITY + 2)

/**
 * @struct LaudVar
 *
 * The structure representing LaudVar objects.
 */
struct LaudVar {
#ifndef VAR_C
  /* Private members to simulate encapsulation */
  char ___[sizeof(struct LaudVar_private {
#endif
    struct LaudPlaceholder _; /**< Parent class for LaudVar. */
    float *values; /**< The float values stored in the LaudVar object. */
    size_t *rank_shape_length; /**< Array holding rank, shape, and length. */
    unsigned char is_continuous : 1; /**< Flag indicating continuity (1 for
                                        continuous, 0 for discontinuous). */
#ifndef VAR_C
  })];
#endif
};

/**
 * @struct LaudVarClass
 *
 * The structure representing the LaudVarClass, which inherits from TypeClass.
 */
struct LaudVarClass {
  struct TypeClass _;             /**< Inherited from TypeClass. */
  void *(*evaluate)(void *self_); /**< A function pointer to the evaluation
                                     function for LaudVar. */
  void (*differentiate)(
      const void *self, void *derivative,
      struct LaudHashMap *ddx); /**< A function pointer for differentiation. */
  char (*is_float)(const void *self); /**< A function pointer to check if
                                         LaudVar holds a float value. */
};

/**
 * @var LaudVarClass
 *
 * External object declaration for the LaudVarClass.
 */
extern const void *LaudVarClass;

/**
 * @var laud_var_protected
 *
 * Structure containing function pointers for protected LaudVar functions.
 */
extern const struct laud_var_protected {
  /**
   * @brief Function to set the float value of a LaudVar object.
   *
   * @param self A pointer to the LaudVar object.
   * @param ndx The index to set the value.
   * @param value The float value to be set.
   * @return The updated float value.
   */
  float (*set_value)(void *self, size_t ndx, float value);

  /**
   * @brief Function to get the float value stored in a LaudVar object.
   *
   * @param self A pointer to the LaudVar object.
   * @param ndx The index to retrieve the value.
   * @return The value at the specified index.
   */
  float (*get_value)(const void *self, size_t ndx);
} laud_var_protected;

/**
 * @brief Function to get a random float value.
 *
 * @return A random value.
 */
float laud_rng();

#endif
