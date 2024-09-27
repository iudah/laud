#ifndef LAUD_MSE_R_H
#define LAUD_MSE_R_H

#include "../../../core/var.r.h"

struct laud_mse_class {
  struct laud_var_class _;
};

struct laud_mse {
  struct laud_var _;
};

#define LAUD_MSE_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudMSE;
extern const void *LaudMSEClass;

#endif