/**
 * @file Slice.r.h
 *
 * Defines structures and declarations related to LaudSlice objects.
 */
#ifndef SLICE_R_H
#define SLICE_R_H

#include <stddef.h>

#include "Var.r.h"

#define LAUD_SLICE_PRIORITY (LAUD_VAR_PRIORITY + 2)

/**
 * @struct LaudSliceObject
 * Represents information about a slice.
 */
struct LaudSliceObject {
  size_t start, end, step, stride;
};

/**
 * @struct LaudSlice
 * Represents a LaudSlice object.
 */
struct LaudSlice {
#ifndef SLICE_C
  char ___[sizeof(struct ___ {
#endif
    struct LaudVar _;               /**< Base LaudVar structure. */
    struct LaudVar *dependency_var; /**< Dependency variable for the slice. */
    struct LaudSliceObject *slice_info; /**< Information about the slice. */

#ifndef SLICE_C
  })];
#endif
};

/**
 * @var LaudSlice
 * Global variable representing the LaudSlice class.
 */
extern const void *LaudSlice;

/**
 * @brief Create a LaudSliceObject based on the slice format.
 *
 * @param self The array from which the slice is created.
 * @param slice The slice format.
 * @param args Variable arguments list.
 * @param new_shape Pointer to store the new shape.
 * @param new_length Pointer to store the new length.
 * @return Pointer to the created LaudSliceObject.
 */
void *create_slice_object(const void *self, const char *const slice,
                          va_list *args, size_t *new_shape, size_t *new_length);

/**
 * @brief Get the slice information from a LaudSlice object.
 *
 * @param self The LaudSlice object.
 * @return Pointer to the LaudSliceObject containing slice information.
 */
const struct LaudSliceObject *get_slice_info(const struct LaudSlice *self);

/**
 * @brief Get the dependency variable from a LaudSlice object.
 *
 * @param self The LaudSlice object.
 * @return Pointer to the dependency variable.
 */
const struct LaudVar *get_slice_dependency(const struct LaudSlice *self);
#endif
