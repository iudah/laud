/**
 * @file Const.h
 *
 * This file contains declarations for the constant LaudConst objects and
 * related functions.
 */
#ifndef CONST_H
#define CONST_H

#include <stddef.h>

#include "Var.h"

/**
 * @var LaudConst
 *
 * External object declaration for the LaudConst class.
 */
extern const void *LaudConst,
    /**
     * @var One
     *
     * External object declaration for the constant value 1.
     */
    *One,
    /**
     * @var MinusOne
     *
     * External object declaration for the constant value -1.
     */
    *MinusOne,
    /**
     * @var Zero
     *
     * External object declaration for the constant value 0.
     */
    *Zero;

/**
 * @brief Create a constant LaudConst object.
 *
 * @param rank The rank of the LaudConst object.
 * @param shape An array specifying the shape of the LaudConst object.
 * @param length The length of the data array.
 * @param data An array containing the constant values for the LaudConst object.
 *
 * @return A pointer to the created LaudConst object.
 */
LAUDAPI void *laud_const(const size_t rank, const size_t *const shape,
                         const size_t lenght, const float *data)
    __attribute__((malloc));

#endif
