#ifndef VAR_R_STATIC_H
#define VAR_R_STATIC_H

#include "narray.h"
#include "var.r.h"

static inline struct laud_narray *narray(const struct laud_var *node) {
  if (is_laud_narray(node))
    return (struct laud_narray *)node;
  return node->value;
}

#endif
