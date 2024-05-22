#ifndef BINARY_CROSS_ENTROPY_R_H
#define BINARY_CROSS_ENTROPY_R_H

#include "../../../core/var.r.h"

struct laud_binary_cross_entropy_class {
  struct laud_var_class _;
};

struct laud_binary_cross_entropy {
  struct laud_var _;
};

#define LAUD_BINARY_CROSS_ENTROPY_PRIORITY (LAUD_VAR_PRIORITY + 3)

extern const void *LaudBinaryCrossEntropy;
extern const void *LaudBinaryCrossEntropyClass;

void *laud_narray_dbinary_cross_entropy(void *operand_a, void *operand_b,
                                        uint64_t respect_index,
                                        const struct laud_narray *pre_dx,
                                        struct laud_narray *calc_result);

#endif