#ifndef LAUD_POOL_X_VAR_H
#define LAUD_POOL_X_VAR_H

#include <Ubject.h>
#include <stdint.h>

#include "../../../core/var.r.static.h"
#include "../../../math/nn/pool/pool.r.h"

static void *var_pool(const struct laud_var *input, const void *pool_class,
                      const uint64_t *pool_size, const uint64_t *stride,
                      const uint64_t *paddings) {
  if ((!pool_class) || pool_class == LaudPooling ||
      super(pool_class) != LaudPooling){
    UbjectError.error("invalid pooling method");
}
  struct laud_var *pool =
      init(pool_class, input, NULL, pool_size, stride, paddings);

  if (input->value) {
    laud_evaluate_var_node(pool);
  }

  return pool;
}

#endif