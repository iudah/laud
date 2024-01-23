/**
 * @file Operator.r.h
 *
 * This file contains declarations for the internal structure of the
 * LaudOperator data type and its class.
 */
#ifndef OPERATOR_R_H
#define OPERATOR_R_H

#include "Var.r.h"
#include "ds/Queue.h"
#include "ds/Stack.h"

#define LAUD_OPERATOR_PRIORITY (LAUD_VAR_PRIORITY + 2)

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
  /**< Base LaudVarClass structure. */
  struct LaudVarClass _;
  /**< Function pointer for computing
             derivatives of LaudOperator objects. */
  void (*compute_derivative)(const void *self, const void *d_var_d_self,
                             struct LaudQueue *derivatives);
  /**< Function pointer to check if an object is a valid
                        dependency for LaudOperator. */
  char (*is_a_valid_dependency)(const void *self, void *dependency,
                                size_t index);
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
   * @brief Retrieve the dependency stack of a LaudOperator.
   *
   * @param operator A pointer to the LaudOperator object.
   * @return A pointer to the dependency stack of the LaudOperator.
   */
  const struct LaudStack *(*dependency)(const void *operator);

  /**
   * @brief Reserve space for dependencies in a LaudOperator object.
   *
   * @param self_ A pointer to the LaudOperator object.
   * @param count Number of dependencies to reserve space for.
   *@return Returns the number of dependencies reserved on success, and 0
   *        otherwise. A return value of 0 may indicate a failure to allocate
   *        memory.
   */
  int (*reserve)(void *self_, int count);

  /**
   * @brief Push a dependency into a LaudOperator object.
   *
   * @param self_ A pointer to the LaudOperator object.
   * @param dependency A pointer to the dependency to be pushed.
   * @return A pointer to the pushed dependency.
   */
  void *(*push)(void *self_, void *dependency);

  /**
   * @brief Check if an object is a LaudOperator.
   *
   * @param self A pointer to the object to be checked.
   * @return 1 if the object is a LaudOperator, 0 otherwise.
   */
  char (*is_operator)(const void *self);

  /**
   * @brief Compute the product of two LaudVar objects.
   *
   * @param x A pointer to the LaudVar result.
   * @param a A pointer to the first LaudVar.
   * @param b A pointer to the second LaudVar.
   */
  void *(*update_respect_product)(const void *x, const void *a, const void *b);

  /**
   * @brief Compute the derivative of a LaudOperator object.
   *
   * @param self A pointer to the LaudOperator object.
   * @param d_var_d_self A pointer to the derivative of the object with respect
   * to itself.
   * @param derivatives A queue to store computed derivatives.
   */
  void (*compute_derivative)(const void *self, const void *d_var_d_self,
                             struct LaudQueue *derivatives);

} LaudOperatorProtected;

#endif
