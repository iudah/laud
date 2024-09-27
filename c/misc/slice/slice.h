#ifndef LAUD_SLICE_H
#define LAUD_SLICE_H

#include "../../core/base.h"

/**
 * Extracts a sub-array from a Laud object using a format string.
 *
 * @param self The Laud object to slice.
 * @param slice_format The format string specifying the slice dimensions.
 * @return A new Laud object containing the extracted sub-array.
 */
LAUDAPI void *laud_slice(const void *self, const char *slice_format, ...)
    __attribute__((format(printf, 2, 3), warn_unused_result));
LAUDAPI void *laud_slice_generator(const void *self, const char *slice_format,
                                   ...)
    __attribute__((format(printf, 2, 3), warn_unused_result));

#endif