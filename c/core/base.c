#include <stdlib.h>
#include <string.h>

#include <Ubject.h>

#include "../math/common/add/add.h"
#include "../math/common/matrix_dot/matrix_dot.h"
#include "../math/nn/binary_cross_entropy/binary_cross_entropy.h"
#include "../math/nn/mse/mse.h"
#include "../math/nn/relu/relu.h"
#include "../math/nn/sigmoid/sigmoid.h"
#include "../math/others/user_elementary_fn/user_elementary_fn.h"
#include "base.h"
#include "base.r.h"

// Static functions declarations

#define STATIC_FUNC_DECL

static void *laud_base_class_ctor(void *self_, va_list *args);

static void *base_operator();

#undef STATIC_FUNC_DECL

// Class Initializer

#define CLASS_INIT

const void *LaudBaseClass = NULL;
const void *LaudBase = NULL;

static void __attribute__((constructor(LAUD_BASE_PRIORITY)))
library_initializer(void) {
  if (!LaudBaseClass)
    LaudBaseClass = init(TypeClass, TypeClass, sizeof(struct laud_base_class),
                         ctor, laud_base_class_ctor, NULL);
  if (!LaudBase)
    LaudBase = init(LaudBaseClass, Ubject, sizeof(struct laud_base), className,
                    "LaudBase",                 // class name
                    laud_add, base_operator,    // addition
                    laud_reduce, base_operator, // reduce
                    NULL);
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

static void *base_operator() {
  UbjectError.warn("unexpected call to base_operator()");
  return NULL;
}

static void *laud_base_class_ctor(void *self_, va_list *args) {
  struct laud_base_class *self = super_ctor(LaudBaseClass, self_, args);

  typedef void (*voidf)();

  voidf selector;
  va_list arg = *args;

  while ((selector = va_arg(arg, voidf))) {

    voidf method = va_arg(arg, voidf);
    if (method) {
      if (selector == (voidf)laud_to_string)
        memcpy(&self->to_string, &method, sizeof(method));
      else if (selector == (voidf)laud_slice)
        memcpy(&self->slice, &method, sizeof(method));
      else if (selector == (voidf)laud_matrix_dot)
        memcpy(&self->matrix_dot, &method, sizeof(method));
      else if (selector == (voidf)laud_add)
        memcpy(&self->add, &method, sizeof(method));
      else if (selector == (voidf)laud_relu)
        memcpy(&self->relu, &method, sizeof(method));
      else if (selector == (voidf)laud_sigmoid)
        memcpy(&self->sigmoid, &method, sizeof(method));
      else if (selector == (voidf)laud_shape)
        memcpy(&self->shape, &method, sizeof(method));
      else if (selector == (voidf)laud_rank)
        memcpy(&self->rank, &method, sizeof(method));
      else if (selector == (voidf)laud_evaluate)
        memcpy(&self->evaluate, &method, sizeof(method));
      else if (selector == (voidf)laud_differentiate)
        memcpy(&self->differentiate, &method, sizeof(method));
      else if (selector == (voidf)laud_reduce)
        memcpy(&self->reduce, &method, sizeof(method));
      else if (selector == (voidf)laud_binary_cross_entropy)
        memcpy(&self->binary_cross_entropy, &method, sizeof(method));
      else if (selector == (voidf)laud_mse)
        memcpy(&self->mse, &method, sizeof(method));
      else if (selector == (voidf)laud_user_elementary_fn)
        memcpy(&self->user_elementary_fn, &method, sizeof(method));
      else if (selector == (voidf)laud_value_at_offset)
        memcpy(&self->value_at_offset, &method, sizeof(method));
    }
  }

  return self;
}
