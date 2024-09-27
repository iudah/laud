#ifndef LAUD_MATRIX_DOT_H
#define LAUD_MATRIX_DOT_H

#include "../../../core/base.h"

/**
 * Computes the matrix dot product of two Laud objects.
 *
 * @param operand_a The first Laud object.
 * @param operand_b The second Laud object.
 * @return A new Laud object containing the result of the matrix dot product.
 */
LAUDAPI void *laud_matrix_dot(void *operand_a, void *operand_b);

#endif