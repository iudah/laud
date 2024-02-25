#ifndef SLICE_H
#define SLICE_H

#include "../core/base.h"

LAUDAPI void *laud_slice_generator(const void *laud_object, const char *fmt,
                                   ...)
    __attribute__((format(printf, 2, 3), warn_unused_result));

#endif