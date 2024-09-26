#include "BaseClass.h"
#include "BaseObject.h"
#include "TypeClass.r.h"
#include "Ubject.r.h"
#include <Ubject.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../../../core/narray.r.h"
#include "../../../../core/node.r.static.h"
#include "../../../../core/var.r.static.h"
#include "../../../../math/nn/pool/max/max_pool.h"
#include "../../../../math/nn/pool/max/max_pool.r.h"

static void *differentiate_maxpool(struct laud_pool *pool,
                                   uint64_t operand_index,
                                   const struct laud_narray *derivative);
void *laud_narray_dmaxpool(const struct laud_narray *input_image,
                           const uint64_t respect_flag,
                           const struct laud_narray *precomputed_derivatives,
                           const struct laud_narray *pooled_result);

static uint64_t max_pooling_algo(const number_t *values, uint64_t length,
                                 const uint64_t *indexes, number_t *pool_result,
                                 const uint64_t pool_result_index, void *meta);
static uint64_t max_pool_meta_size(const uint64_t image_rank,
                                   const uint64_t *image_shape,
                                   const uint64_t output_rank,
                                   const uint64_t *output_shape);
static uint64_t
dmax_pooling_algo(const number_t *values, uint64_t indexes_length,
                  const uint64_t *indexes, const uint64_t derivative_idx,
                  number_t *derivative,
                  const uint64_t precomputed_derivative_idx,
                  const number_t precomputed_derivative, void *meta);

const void *LaudMaxPooling = NULL;
const void *LaudMaxPoolingClass = NULL;

static void fini_lib() { FREE((void *)LaudMaxPooling); }

static void __attribute__((__constructor__(LAUD_MAX_POOL_PRIORITY)))
init_lib() {
  if (!LaudMaxPoolingClass)
    LaudMaxPoolingClass = LaudPoolingClass;

  if (!LaudMaxPooling)
    LaudMaxPooling =
        init(LaudMaxPoolingClass, LaudPooling, sizeof(struct laud_max_pool), //
             className, "LaudMaxPooling",                                    //
             laud_pool_compute_fn, max_pooling_algo,                         //
             //  laud_differentiate_var_node, differentiate_maxpool, //
             laud_pool_meta_size, max_pool_meta_size,       //
             laud_pool_differentiate_fn, dmax_pooling_algo, //
             NULL);

  atexit(fini_lib);
}

void *laud_max_pool(void *input, const uint64_t *max_pool_size,
                    const uint64_t *strides, const uint64_t *paddings) {
  const struct laud_base_class *class = classOf(input);
  return class->pool(input, LaudMaxPooling, max_pool_size, strides, paddings,
                     NULL);
}

uint64_t max_pooling_algo(const number_t *values, uint64_t length,
                          const uint64_t *indexes, number_t *pool_result,
                          const uint64_t pool_result_index, void *meta) {

  uint64_t *max_index = meta;

  number_t max = values[max_index[pool_result_index] = indexes[--length]];
  while (length) {
    --length;
    if (max < values[indexes[length]])
      max = values[max_index[pool_result_index] = indexes[length]];
  }
  *pool_result = max;
  return 0;
}

static uint64_t max_pool_meta_size(const uint64_t image_rank,
                                   const uint64_t *image_shape,
                                   const uint64_t output_rank,
                                   const uint64_t *output_shape) {
  uint64_t length = 1;
  for (uint64_t i = 0; i < output_rank; i++) {
    length *= output_shape[i];
  }
  return length * sizeof(number_t);
}

static void *differentiate_maxpool(struct laud_pool *pool,
                                   uint64_t operand_index,
                                   const struct laud_narray *derivative) {
  return laud_narray_dmaxpool(
      narray((struct laud_var *)incoming_nodes(pool)[0]), operand_index,
      derivative, narray((void *)pool));
}

static uint64_t
dmax_pooling_algo(const number_t *values, uint64_t indexes_length,
                  const uint64_t *indexes, const uint64_t derivative_idx,
                  number_t *derivative,
                  const uint64_t precomputed_derivative_idx,
                  const number_t precomputed_derivative, void *meta) {

  uint64_t *max_index = meta;

  *derivative = (max_index[precomputed_derivative_idx] == derivative_idx)
                    ? precomputed_derivative
                    : 0;

  return 0;
}