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
#include "../../../core/narray.r.h"
#include "../../../core/node.r.static.h"
#include "../../../core/var.r.static.h"
#include "../../../math/nn/pool/pool.h"
#include "../../../math/nn/pool/pool.r.h"

static void *laud_pool_class_ctor(void *self_, va_list *args);
static void *laud_pool_ctor(void *pool_instance, va_list *args);
static void *laud_pool_dtor(struct laud_pool *pool_instance);
static void *solve_pool(struct laud_pool *pool);
static void *differentiate_pool(struct laud_pool *pool, uint64_t operand_index,
                                const struct laud_narray *derivative);

const void *LaudPooling = NULL;
const void *LaudPoolingClass = NULL;

static void fini_lib() {
  FREE((void *)LaudPooling);
  FREE((void *)LaudPoolingClass);
}

static void __attribute__((__constructor__(LAUD_POOL_PRIORITY))) init_lib() {
  if (!LaudPoolingClass)
    LaudPoolingClass =
        init(TypeClass, LaudVarClass, sizeof(struct laud_pool_class), ctor,
             laud_pool_class_ctor, NULL);

  if (!LaudPooling)
    LaudPooling = init(LaudPoolingClass, LaudVar, sizeof(struct laud_pool), //
                       className, "LaudPooling",                            //
                       ctor, laud_pool_ctor,                                //
                       dtor, laud_pool_dtor,                                //
                       laud_evaluate_var_node, solve_pool,              // solve
                       laud_differentiate_var_node, differentiate_pool, //
                       NULL);

  atexit(fini_lib);
}

static void *laud_pool_class_ctor(void *self_, va_list *args) {
  struct laud_pool_class *self = super_ctor(LaudPoolingClass, self_, args);

  typedef void (*voidf)();

  voidf selector;
  va_list arg = *args;

  while ((selector = va_arg(arg, voidf))) {

    voidf method = va_arg(arg, voidf);
    if (method) {
      if (selector == (voidf)laud_pool_compute_fn)
        memcpy(&self->pooling_algo, &method, sizeof(method));
      else if (selector == (voidf)laud_pool_differentiate_fn)
        memcpy(&self->pooling_deriv, &method, sizeof(method));
      else if (selector == (voidf)laud_pool_meta_size)
        memcpy(&self->pooling_meta_size, &method, sizeof(method));
    }
  }

  return self;
}

void *laud_pool(void *input, const void *pool_class, const uint64_t *pool_size,
                const uint64_t *strides, const uint64_t *paddings) {
  const struct laud_base_class *class = classOf(input);
  return class->pool(input, pool_class, pool_size, strides, paddings, NULL);
}

static void *laud_pool_ctor(void *pool_instance, va_list *args) {
  struct laud_pool *pool = super_ctor(LaudPooling, pool_instance, args);

  pool->pool_size = va_arg(*args, uint64_t *);
  pool->strides = va_arg(*args, uint64_t *);
  pool->paddings = va_arg(*args, uint64_t *);

  return pool;
}

static void *laud_pool_dtor(struct laud_pool *pool) {

  if (pool->paddings) {
    FREE(pool->paddings);
  }
  if (pool->strides) {
    FREE(pool->strides);
  }
  return super_dtor(LaudPooling, pool);
}

static void *solve_pool(struct laud_pool *pool) {
  return laud_pool(narray((struct laud_var *)incoming_nodes(pool)[0]),
                   classOf(pool), pool->pool_size, pool->strides,
                   pool->paddings);
}

static void *differentiate_pool(struct laud_pool *pool, uint64_t operand_index,
                                const struct laud_narray *derivative) {
  return laud_narray_dpool(
      narray((struct laud_var *)incoming_nodes(pool)[0]),
      // narray((struct laud_var *)incoming_nodes(pool)[1]),
      operand_index, derivative, classOf(pool), narray((void *)pool));
}

laud_pool_compute_fn_t laud_pool_compute_fn(const void *meta) {
  return ((struct laud_pool_class *)meta)->pooling_algo;
}

laud_pool_differentiate_fn_t laud_pool_differentiate_fn(const void *meta) {
  return ((struct laud_pool_class *)meta)->pooling_deriv;
}

uint64_t laud_pool_meta_size(const void *pool_class, const uint64_t image_rank,
                             const uint64_t *image_shape,
                             const uint64_t output_rank,
                             const uint64_t *output_shape) {
  return ((struct laud_pool_class *)pool_class)
      ->pooling_meta_size(image_rank, image_shape, output_rank, output_shape);
}
