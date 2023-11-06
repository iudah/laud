/**
 * @file Const.h
 *
 * This file contains declarations for the constant LaudConst objects and
 * related functions.
 */
#ifndef CONST_H
#define CONST_H
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
 * @brief Function to create a LaudConst object with a specified value.
 *
 * This function initializes and returns a LaudConst object with the provided
 * value.
 *
 * @param value The float value to set for the LaudConst object.
 * @return A pointer to the created LaudConst object.
 */
void *LaudConst_(float value);

#endif