#ifndef VAR_R_H
#define VAR_R_H

#include "narray.r.h"
#include "node.r.h"

struct laud_var;

struct laud_var_class {
  struct laud_node_class _;
  void *(*evaluate_node)(struct laud_var *var);
};

struct laud_var {
  struct laud_node _;
  struct laud_narray *value;
  struct laud_narray *derivative;
};

#define LAUD_VAR_PRIORITY (LAUD_NODE_PRIORITY + 3)

extern const void *LaudVar;
extern const void *LaudVarClass;

#ifdef VAR_PROTECTED

#include "narray.h"

static inline struct laud_narray *evaluate_node(void *node) {
  const struct laud_var_class *class = classOf(node);
  return class->evaluate_node(node);
}

static inline struct laud_narray *narray(struct laud_var *node) {
  if (is_laud_narray(node))
    return (struct laud_narray *)node;
  return node->value;
}

#endif

#endif