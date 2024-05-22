#ifndef ADD_H
#define ADD_H

#include "../../../core/base.h"

/**
 * Performs element-wise addition between two Laud objects.
 *
 * @param operand_a The first Laud object.
 * @param operand_b The second Laud object.
 * @return A new Laud object containing the result of the addition.
 */
LAUDAPI void *laud_add(void *operand_a, void *operand_b);

#endif