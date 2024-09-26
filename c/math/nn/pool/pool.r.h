#ifndef LAUD_POOL_R_H
#define LAUD_POOL_R_H

#include "../../../core/var.r.h"
#include <stdint.h>

#define LAUD_POOL_PRIORITY (LAUD_VAR_PRIORITY + 3)

typedef uint64_t (*laud_pool_compute_fn_t)(const number_t *input_values,
                                           uint64_t indexes_length,
                                           const uint64_t *indexes,
                                           number_t *pool_result,
                                           const uint64_t pool_result_index,
                                           void *meta);
typedef uint64_t (*laud_pool_differentiate_fn_t)(
    const number_t *input_values, uint64_t indexes_length,
    const uint64_t *indexes, const uint64_t derivative_idx,
    const number_t *compute_derivative,
    const uint64_t precomputed_derivative_idx,
    const number_t precomputed_derivative, void *meta);

struct laud_pool_class {
  struct laud_var_class _;
  laud_pool_compute_fn_t pooling_algo;
  laud_pool_differentiate_fn_t pooling_deriv;
  uint64_t (*pooling_meta_size)(const uint64_t image_rank,
                                const uint64_t *image_shape,
                                const uint64_t output_rank,
                                const uint64_t *output_shape);
};

struct laud_pool {
  struct laud_var _;
  uint64_t *pool_size;
  uint64_t *strides;
  uint64_t *paddings;
};

extern const void *LaudPooling;
extern const void *LaudPoolingClass;

void *laud_narray_dpool(const struct laud_narray *operand_a,
                        const uint64_t respect_index,
                        const struct laud_narray *pre_dx,
                        const void *pool_class,
                        const struct laud_narray *calc_result);
laud_pool_compute_fn_t laud_pool_compute_fn(const void *pooling_class);
laud_pool_differentiate_fn_t
laud_pool_differentiate_fn(const void *pooling_class);
uint64_t laud_pool_meta_size(const void *pool_class, const uint64_t image_rank,
                             const uint64_t *image_shape,
                             const uint64_t output_rank,
                             const uint64_t *output_shape);
#endif
