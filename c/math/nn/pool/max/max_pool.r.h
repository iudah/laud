#ifndef LAUD_MAX_POOL_R_H
#define LAUD_MAX_POOL_R_H

#include "../pool.r.h"
#include <stdint.h>

#define LAUD_MAX_POOL_PRIORITY (LAUD_POOL_PRIORITY + 3)

struct laud_max_pool_class {
  struct laud_pool_class _;
};

struct laud_max_pool {
  struct laud_pool _;
};

extern const void *LaudMaxPooling;
extern const void *LaudMaxPoolingClass;

// void *laud_narray_dmax_pool(const struct laud_narray *operand_a,
//                         uint64_t respect_index,
//                         const struct laud_narray *pre_dx,
//                         struct laud_narray *calc_result);

#endif