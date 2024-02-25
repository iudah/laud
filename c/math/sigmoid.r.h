#ifndef SIGMOID_R_H
#define SIGMOID_R_H

#include "../core/var.r.h"

struct laud_sigmoid_class {
  struct laud_var_class _;
};

struct laud_sigmoid {
  struct laud_var _;
};

#define LAUD_SIGMOID_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudSigmoid;
extern const void *LaudSigmoidClass;

#endif