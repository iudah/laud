#ifndef LAUD_CONV_H
#define LAUD_CONV_H

#include <stdint.h>

#include "../../../core/base.h"

LAUDAPI void *laud_conv(void *input, void *kernel, const uint64_t *strides,
                        const uint64_t *paddings)
    __attribute__((warn_unused_result));

#endif