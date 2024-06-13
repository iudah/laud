#ifndef REDUCE_R_H
#define REDUCE_R_H

#include "../../core/var.r.h"

struct laud_reduce_class {
  struct laud_var_class _;
};

struct laud_reduce {
  struct laud_var _;
  number_t (*callback)(const number_t current_net, const number_t *const values,
                       const void *args);
  void *args;
  int16_t axis;
};

#define LAUD_REDUCE_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudReduce;
extern const void *LaudReduceClass;

#endif