#ifndef MATRIX_DOT_R_H
#define MATRIX_DOT_R_H

#include "../core/var.r.h"

struct laud_matrix_dot_class {
  struct laud_var_class _;
};

struct laud_matrix_dot {
  struct laud_var _;
};

#define LAUD_MATRIX_DOT_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudMatrixDot;
extern const void *LaudMatrixDotClass;

#endif