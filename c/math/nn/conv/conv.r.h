#ifndef LAUD_CONV_R_H
#define LAUD_CONV_R_H

#include "../../../core/var.r.h"
#include <stdint.h>

#define LAUD_CONV_PRIORITY (LAUD_VAR_PRIORITY + 3)

struct laud_conv_class {
  struct laud_var_class _;
};

struct laud_conv {
  struct laud_var _;
  uint64_t *strides;
  uint64_t *paddings;
};

extern const void *LaudConvolution;
extern const void *LaudConvolutionClass;

void *laud_narray_dconv(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b,
                        uint64_t respect_index,
                        const struct laud_narray *pre_dx,
                        struct laud_narray *calc_result);

#endif