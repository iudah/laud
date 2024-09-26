#ifndef LAUD_CONV_X_VAR_H
#define LAUD_CONV_X_VAR_H

#include <Ubject.h>
#include <stdint.h>

#include "../../../core/var.r.static.h"
#include "../../../math/nn/conv/conv.r.h"

static void *var_conv(const struct laud_var *input,
                      const struct laud_var *kernel, const uint64_t *stride,
                      const uint64_t *paddings) {

  struct laud_var *conv =
      init(LaudConvolution, input, kernel, NULL, stride, paddings);

  if (input->value && kernel->value) {
    laud_evaluate_var_node(conv);
  }

  return conv;
}

#endif