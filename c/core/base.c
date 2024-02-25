#include <stdlib.h>

#include <Ubject.h>

#include "../math/nn_activations.h"
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
                    "LaudBase",              // class name
                    laud_add, base_operator, // addition
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
        *(voidf *)&self->to_string = method;
      else if (selector == (voidf)laud_slice)
        *(voidf *)&self->slice = method;
      else if (selector == (voidf)laud_matrix_dot)
        *(voidf *)&self->matrix_dot = method;
      else if (selector == (voidf)laud_add)
        *(voidf *)&self->add = method;
      else if (selector == (voidf)laud_relu)
        *(voidf *)&self->relu = method;
      else if (selector == (voidf)laud_sigmoid)
        *(voidf *)&self->sigmoid = method;
      else if (selector == (voidf)laud_shape)
        *(voidf *)&self->shape = method;
      else if (selector == (voidf)laud_rank)
        *(voidf *)&self->rank = method;
      else if (selector == (voidf)laud_evaluate)
        *(voidf *)&self->evaluate = method;
    }
  }

  return self;
}
