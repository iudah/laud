#ifndef LAUD_POOL_H
#define LAUD_POOL_H

#include <stdint.h>

#include "../../../core/base.h"

LAUDAPI void *laud_pool(void *input, const void *pool_class,
                        const uint64_t *pool_size, const uint64_t *strides,
                        const uint64_t *paddings)
    __attribute__((warn_unused_result));

#endif