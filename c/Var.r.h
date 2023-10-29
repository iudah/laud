/**
 * @file Var.r.h
 *
 * This file defines the structure and declarations related to LaudVar objects.
 */
#ifndef VAR_R_H
#define VAR_R_H

#include "../../Ubject/Ubject.r.h"
#include "ds/Queue.h"

/**
 * @struct LaudVar
 *
 * The structure representing LaudVar objects.
 */
struct LaudVar {
#ifndef VAR_C
  char ___[sizeof(struct __ {
#endif
    struct Ubject _; /**< Inherited from the Ubject class. */
    float value;     /**< The float value stored in the LaudVar object. */
    char discontinuity_marker : 1; /**< A marker indicating discontinuity (1 for
                                      discontinuous, 0 for continuous). */
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
      struct LaudQueue *ddx); /**< A function pointer for differentiation. */
  char (*isFloat)(const void *self); /**< A function pointer to check if LaudVar
                                        holds a float value. */
};

/**
 * @brief Function to evaluate a LaudVar object.
 *
 * @param self A pointer to the LaudVar object to be evaluated.
 * @return A pointer to the result of the evaluation.
 */
static void *LaudVar_evaluate(void *self);
/**
 * @brief Function to compute the derivative of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @param derivative A pointer to the derivative object.
 * @param ddx An array of pointers to store the derivatives.
 */
static void LaudVar_differentiate(const void *self, const void *derivative,
                                  struct LaudQueue *ddx);
/**
 * @var LaudVarClass
 *
 * External object declaration for the LaudVarClass.
 */
extern const void *LaudVarClass;

/**
 * @var LaudVarProtected
 *
 * Structure containing function pointers for protected LaudVar functions.
 */
extern const struct LaudVarProtected {
  /**
   * @brief Function to set the float value of a LaudVar object.
   *
   * @param self A pointer to the LaudVar object.
   * @param value The float value to be set.
   * @return The updated float value.
   */
  float (*setValue)(void *self, float value);
  /**
   * @brief Function to get the float value stored in a LaudVar object.
   *
   * @param self A pointer to the LaudVar object.
   * @return The float value stored in the LaudVar object.
   */
  float (*getValue)(const void *self);
} LaudVarProtected;
#endif