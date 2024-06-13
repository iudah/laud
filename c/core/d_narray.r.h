#ifndef D_NARRAY_R_H
#define D_NARRAY_R_H

/**
 * @file d_narray.r.h
 * @brief Header file declaring functions for differentiating N-dimensional
 * arrays in Laud.
 */


#include <stddef.h>

#include "./narray.r.h"

void *laud_narray_dreduce(const struct laud_narray *operand_a,
                          uint64_t __attribute__((__unused__)) respect_index,
                          struct laud_narray *pre_dx,
                          struct laud_narray *calc_result);

void *laud_narray_dmse(void *operand_a, void *operand_b, uint64_t respect_index,
                       const struct laud_narray *pre_dx,
                       const struct laud_narray *calc_result);
#endif
