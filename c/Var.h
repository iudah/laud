/**
 * @file Var.h
 *
 * This file contains declarations for the LaudVar data type and associated
 * functions.
 */
#ifndef VAR_H
#define VAR_H

#include "../../Ubject/Ubject.h"

/**
 * @brief External object declaration for LaudVar.
 */
extern const void *LaudVar;

/**
 * @brief Create a LaudVar object with a float value.
 *
 * @param f The float value to be stored in the LaudVar object.
 * @return A pointer to the newly created LaudVar object.
 */
void *LaudVarVar(const float f);

/**
 * @brief Delete a LaudVar object and release allocated resources.
 *
 * @param self A pointer to the LaudVar object to be deleted.
 */
void LaudVarDelete(void *self);

/**
 * @brief Create a copy of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object to be copied.
 * @return A pointer to the newly created copy of the LaudVar object.
 */
void *LaudVarCopy(const void *self);

/**
 * @brief Set the float value of a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @param value The float value to be set.
 * @return The updated float value.
 */
float LaudVarSetFloatValue(void *self, const float value);

/**
 * @brief Check if a LaudVar object holds a float value.
 *
 * @param self A pointer to the LaudVar object.
 * @return 1 if the LaudVar object holds a float value, 0 otherwise.
 */
char LaudVarIsFloat(const void *self);

/**
 * @brief Retrieve the float value stored in a LaudVar object.
 *
 * @param self A pointer to the LaudVar object.
 * @return The float value stored in the LaudVar object.
 */
float LaudVarGetFloatValue(const void *self);

/**
 * @brief Evaluate a LaudVar object and return a pointer to the result.
 *
 * @param self A pointer to the LaudVar object to be evaluated.
 * @return A pointer to the result of the evaluation.
 */
void *LaudVarEvaluate(void *self);

/**
 * @brief Compute the derivative of a LaudVar object with respect to itself.
 *
 * @param self A pointer to the LaudVar object.
 * @param derivative A pointer to the derivative object.
 * @param ddx A map of pointers to store the derivatives.
 * @return The number of derivatives computed.
 */
int LaudVarDifferentiate(const void *self, void *derivative, void *ddx);
void *LaudVarDerivativeMap();
void LaudVarDeleteDerivativeMap(void *map);
void LaudVarDerivativeMapIterStart(void *map);
void **LaudVarDerivativeMapNext(void *map);
// void *LaudDerivativeGraph(const void *self);
/**
 * @brief Set the continuity of a LaudVar object (continuous or discontinuous).
 *
 * @param self_ A pointer to the LaudVar object.
 * @param continuous Flag indicating continuity (1 for continuous, 0 for
 * discontinuous).
 */
void LaudVarSetContinuity(void *self_, char continous);
/**
 * @brief Check if a LaudVar object is continuous.
 *
 * @param self A pointer to the LaudVar object.
 * @return 1 if the LaudVar object is continuous, 0 if it is discontinuous.
 */
char LaudVarIsContinous(const void *self);

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
extern const void *LaudQoutient;

/**
 * @brief Perform addition between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * addition.
 */
void *LaudVarAdd(const void *a, const void *b);

/**
 * @brief Perform subtraction between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * subtraction.
 */
void *LaudVarMinus(const void *a, const void *b);

/**
 * @brief Perform multiplication between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * multiplication.
 */
void *LaudVarProduct(const void *a, const void *b);

/**
 * @brief Perform division between two LaudVar objects.
 *
 * @param a A pointer to the first LaudVar object.
 * @param b A pointer to the second LaudVar object.
 * @return A pointer to the LaudVar object representing the result of the
 * division.
 */
void *LaudVarQoutient(const void *a, const void *b);
#endif