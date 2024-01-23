#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>

#include <Ubject.h>

#define PLACEHOLDER_C
#include "Placeholder.r.h"
#undef PLACEHOLDER_C

const void *LaudPlaceholderClass = NULL, *LaudPlaceholder = NULL,
           *LAUD_PLACEHOLDER = NULL;

static void __attribute__((constructor(LAUD_PLACEHOLDER_PRIORITY)))
init_laud_placeholder(void) {
  if (!LaudPlaceholderClass) {
    LaudPlaceholderClass =
        init(TypeClass, TypeClass, sizeof(struct LaudPlaceholderClass), NULL);
  }

  if (!LaudPlaceholder) {
    LaudPlaceholder =
        init(LaudPlaceholderClass, Ubject, sizeof(struct LaudPlaceholder),
             className, "LaudPlaceholder", NULL);
  }

  if (!LAUD_PLACEHOLDER) {
    LAUD_PLACEHOLDER = init(LaudPlaceholder);
    reference((void *)LAUD_PLACEHOLDER);
  }
}
