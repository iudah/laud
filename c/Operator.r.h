/**
 * @file Operator.r.h
 *
 * This file contains declarations for the internal structure of the
 * LaudOperator data type and its class.
 */
#ifndef OPERATOR_R_H
#define OPERATOR_R_H

#include "Var.r.h"
#include "ds/Stack.h"

/**
 * @var LaudOperator
 *
 * External object declaration for the LaudOperator class.
 */
extern const void *LaudOperator;

/**
 * @var LaudOperatorClass
 *
 * External object declaration for the LaudOperatorClass, derived from
 * LaudVarClass.
 */
extern const void *LaudOperatorClass;

/**
 * @struct LaudOperatorClass
 *
 * Structure representing the class of LaudOperator with function pointers.
 */
struct LaudOperatorClass {
  struct LaudVarClass _; /**< Base LaudVarClass structure. */
  const void *(*computeDerivative)(
      const void *self, const void *d_var_d_self,
      struct LaudQueue *derivatives); /**< Function pointer for computing
            derivatives of LaudOperator objects. */
};

/**
 * @struct LaudOperator
 *
 * Internal structure representing a LaudOperator object.
 */
struct LaudOperator {
#ifndef OPERATOR_C
  char ___[sizeof(struct ___ {
#endif
    struct LaudVar _; /**< Base LaudVar structure. */
    struct LaudStack
        *dependency;  /**< Stack to store dependencies of the LaudOperator. */
    void *derivative; /**< Pointer to the derivative object. */
    char derivativeRespected : 1; /**< Derivative respect marker (1 for
                                     respected, 0 for not respected). */

#ifndef OPERATOR_C
  })];
#endif
};

extern const struct LaudOperatorProtected {

  /**
   * @brief Function to retrieve the dependency of a LaudOperator.
   *
   * @param operator A pointer to the LaudOperator object.
   * @return A pointer to the dependency stack of the LaudOperator.
   */
  const void *(*dependency)(const void *operator);

  /**
   * @brief Function to reserve space for dependencies in a LaudOperator object.
   *
   * @param self_ A pointer to the LaudOperator object.
   * @param count Number of dependencies to reserve space for.
   */
  int (*reserve)(void *self_, int count);

  /**
   * @brief Function to push a dependency into a LaudOperator object.
   *
   * @param self_ A pointer to the LaudOperator object.
   * @param dependency A pointer to the dependency to be pushed.
   */
  void *(*push)(void *self_, void *dependency);

  /**
   * @brief Function to check if an object is a LaudOperator.
   *
   * @param self A pointer to the object to be checked.
   * @return 1 if the object is a LaudOperator, 0 otherwise.
   */
  char (*is_operator)(const void *self);

  /**
   * @brief Function to compute the product of two LaudVar objects.
   *
   * @param a A pointer to the first LaudVar.
   * @param b A pointer to the second LaudVar.
   * @return A pointer to the product of the two LaudVar objects.
   */
  void *(*update_respect_product)(const void *a, const void *b);
  /**
   * @brief Function to compute the derivative of a LaudOperator object.
   *
   * @param self A pointer to the LaudOperator object.
   * @param d_var_d_self A pointer to the derivative of the object with respect
   * to itself.
   * @param derivatives A queue to store computed derivatives.
   * @return A pointer to the LaudOperator object.
   */
  const void *(*ComputeDerivative)(const void *self, const void *d_var_d_self,
                                   struct LaudQueue *derivatives);

} LaudOperatorProtected;

/**
 * @brief Function to compute the derivative of a LaudOperator object.
 *
 * This function is to be overridden by subclasses to provide custom
 * derivative computation logic.
 *
 * @param self A pointer to the LaudOperator object.
 * @param d_var_d_self A pointer to the derivative of the object with respect
 * to itself.
 * @param derivatives A queue to store computed derivatives.
 * @return A pointer to the LaudOperator object.
 */
static const void *
LaudOperator_computeDerivative(const void *self, const void *d_var_d_self,
                               struct LaudQueue *derivatives);
#endif