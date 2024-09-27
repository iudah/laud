#ifndef LAUD_RELU_R_H
#define LAUD_RELU_R_H

#include "../../../core/var.r.h"

struct laud_relu_class {
  struct laud_var_class _;
};

struct laud_relu {
  struct laud_var _;
};

#define LAUD_RELU_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudReLU;
extern const void *LaudReLUClass;

void *laud_narray_drelu(const struct laud_narray *operand_a,
                        uint64_t __attribute__((__unused__)) respect_index,
                        const struct laud_narray *pre_dx,
                        const struct laud_narray *calc_result);

#endif