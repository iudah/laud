#ifndef LAUD_VAR_R_H
#define LAUD_VAR_R_H

#include "narray.r.h"
#include "node.r.h"

struct laud_var;

struct laud_var_class {
  struct laud_node_class _;
  void *(*evaluate_node)(struct laud_var *var);
  void *(*differentiate_node)(struct laud_var *node_var, uint64_t operand_index,
                              const struct laud_narray *derivative);
};

struct laud_var {

  struct laud_node _;
  struct laud_narray *value;
  struct laud_narray *derivative;
};

#define LAUD_VAR_PRIORITY (LAUD_NODE_PRIORITY + 3)

extern const void *LaudVar;
extern const void *LaudVarClass;

struct laud_narray *laud_evaluate_var_node(void *node);
struct laud_narray *
laud_differentiate_var_node(struct laud_var *node, uint64_t operand_index,
                            const struct laud_narray *derivative);

#endif