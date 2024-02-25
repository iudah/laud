#ifndef RELU_R_H
#define RELU_R_H

#include "../core/var.r.h"

struct laud_relu_class {
  struct laud_var_class _;
};

struct laud_relu {
  struct laud_var _;
};

#define LAUD_RELU_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudReLU;
extern const void *LaudReLUClass;

#endif