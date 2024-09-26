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
#include "../../../math/nn/conv/conv.h"
#include "../../../math/nn/conv/conv.r.h"

static void *laud_conv_ctor(void *conv_instance, va_list *args);
static void *laud_conv_dtor(struct laud_conv *conv_instance);
static void *solve_conv(struct laud_conv *conv);
static void *differentiate_conv(struct laud_conv *conv, uint64_t operand_index,
                                const struct laud_narray *derivative);

const void *LaudConvolution = NULL;
const void *LaudConvolutionClass = NULL;

static void fini_lib() { FREE((void *)LaudConvolution); }

static void __attribute__((__constructor__(LAUD_CONV_PRIORITY))) init_lib() {
  if (!LaudConvolutionClass)
    LaudConvolutionClass = LaudVarClass;

  if (!LaudConvolution)
    LaudConvolution =
        init(LaudConvolutionClass, LaudVar, sizeof(struct laud_conv), //
             className, "LaudConvolution",                            //
             ctor, laud_conv_ctor,                                    //
             dtor, laud_conv_dtor,                                    //
             laud_evaluate_var_node, solve_conv,                      // solve
             laud_differentiate_var_node, differentiate_conv,         //
             NULL);

  atexit(fini_lib);
}

void *laud_conv(void *input, void *kernel, const uint64_t *strides,
                const uint64_t *paddings) {
  const struct laud_base_class *class = classOf(input);
  return class->conv(input, kernel, strides, paddings, NULL);
}

static void *laud_conv_ctor(void *conv_instance, va_list *args) {
  struct laud_conv *conv = super_ctor(LaudConvolution, conv_instance, args);

  conv->strides = va_arg(*args, uint64_t *);
  conv->paddings = va_arg(*args, uint64_t *);

  return conv;
}

static void *laud_conv_dtor(struct laud_conv *conv) {

  if (conv->paddings) {
    FREE(conv->paddings);
  }
  if (conv->strides) {
    FREE(conv->strides);
  }
  return super_dtor(LaudConvolution, conv);
}

static void *solve_conv(struct laud_conv *conv) {
  return laud_conv(narray((struct laud_var *)incoming_nodes(conv)[0]),
                   narray((struct laud_var *)incoming_nodes(conv)[1]),
                   conv->strides, conv->paddings);
}

static void *differentiate_conv(struct laud_conv *conv, uint64_t operand_index,
                                const struct laud_narray *derivative) {
  return laud_narray_dconv(narray((struct laud_var *)incoming_nodes(conv)[0]),
                           narray((struct laud_var *)incoming_nodes(conv)[1]),
                           operand_index, derivative, narray((void *)conv));
}