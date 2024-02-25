#ifndef ADD_R_H
#define ADD_R_H

#include "../core/var.r.h"

struct laud_add_class {
  struct laud_var_class _;
};

struct laud_add {
  struct laud_var _;
};

#define LAUD_ADD_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudAdd;
extern const void *LaudAddClass;

#endif