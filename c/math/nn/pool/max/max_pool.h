#ifndef LAUD_MAX_POOL_H
#define LAUD_MAX_POOL_H

#include <stdint.h>

#include "../../../../core/base.h"

LAUDAPI void *laud_max_pool(void *input, const uint64_t *max_pool_size,
                            const uint64_t *strides, const uint64_t *paddings)
    __attribute__((warn_unused_result));

#endif