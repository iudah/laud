/**
 * @file Placeholder.r.h
 *
 * This file defines the structure and declarations related to LaudPlaceholder
 * objects.
 */
#ifndef PLACEHOLDER_R_H
#define PLACEHOLDER_R_H

#include <TypeClass.r.h>
#include <Ubject.r.h>

// data structures

#define LAUD_PLACEHOLDER_PRIORITY (UBJECT_PRIORITY + 2)

/**
 * @struct LaudPlaceholder
 *
 * The structure representing LaudPlaceholder objects.
 */
struct LaudPlaceholder {
#ifndef PLACEHOLDER_C
  char ___[sizeof(struct __ {
#endif
    struct Ubject _; /**< Inherited from the Ubject class. */
#ifndef PLACEHOLDER_C
  })];
#endif
};

/**
 * @struct LaudPlaceholderClass
 *
 * The structure representing the LaudPlaceholderClass, which inherits from
 * TypeClass.
 */
struct LaudPlaceholderClass {
  struct TypeClass _; /**< Inherited from TypeClass. */
};

extern const void *LaudPlaceholderClass;
/**
 * @brief External object declaration for LaudPlaceholder.
 */
extern const void *LaudPlaceholder;

#endif