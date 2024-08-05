#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ubject.h>

#define LAUD_VAR_IMPLEMENTATION
#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../core/base.h"
#include "../core/node.r.static.h"
#include "../core/var.h"
#include "../core/var.r.static.h"
#include "../math/common/add/add.h"
#include "../math/common/add/add.x.var.h"
#include "../math/common/matrix_dot/matrix_dot.h"
#include "../math/common/matrix_dot/matrix_dot.xtern.h"
#include "../math/nn/nn.h"
#include "../math/nn/nn.xtern.h"
#include "../math/others/reduce.r.h"
#include "../math/others/user_elementary_fn/user_elementary_fn.h"
#include "../math/others/user_elementary_fn/user_elementary_fn.xtern.h"
#include "../misc/slice/slice.h"
#include "../misc/slice/slice.x.var.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_var(struct laud_var *var);

static void *laud_var_ctor(void *self, va_list *args);

static void *laud_var_dtor(void *self);

static void *laud_var_class_ctor(void *self_, va_list *args);

static char *var_to_string(const void *laud_object, char *buffer,
                           uint64_t buf_limit);

static void *var_reduce(const struct laud_var *operand, uint16_t axis,
                        number_t (*callback)(const number_t current_net,
                                             const number_t *const values,
                                             const void *args),
                        const void *args, void *null);

static const uint64_t *var_shape(const struct laud_var *var);

static uint16_t var_rank(const struct laud_var *var);

static void var_evaluate(const struct laud_var *var);

static void var_differentiate(struct laud_var *var,
                              const struct laud_narray *derivative);

static void *differentiate_var(struct laud_var *var, uint64_t index,
                               struct laud_narray *pre_dx);

static number_t value_at_offset(void *operand, uint64_t offset);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudVar = NULL;
const void *LaudVarClass = NULL;

static void finish_lib() {
  FREE((void*)LaudVar);
  FREE((void*)LaudVarClass);
}

static void __attribute__((constructor(LAUD_VAR_PRIORITY)))
library_initializer(void) {
  if (!LaudVarClass) {
    LaudVarClass = init(TypeClass, LaudNodeClass, sizeof(struct laud_var_class),
                        ctor, laud_var_class_ctor, NULL);
  }
  if (!LaudVar) {
    LaudVar = init(
        LaudVarClass, LaudNode,
        sizeof(struct laud_var),                        // class parent size
        className, "LaudVar",                           // class name
        laud_evaluate_var_node, solve_var,              // evaluate_node
        ctor, laud_var_ctor,                            // constructor
        laud_to_string, var_to_string,                  // to string
        laud_slice, var_slice,                          // slice
        laud_matrix_dot, var_matrix_dot,                // matrix dot
        laud_add, var_add,                              // addition
        laud_relu, var_relu,                            // relu
        laud_sigmoid, var_sigmoid,                      // sigmoid
        laud_shape, var_shape,                          // shape
        laud_rank, var_rank,                            // rank
        laud_evaluate, var_evaluate,                    // evaluate
        laud_differentiate, var_differentiate,          // differentiate
        laud_differentiate_var_node, differentiate_var, // differentiate_node
        laud_reduce, var_reduce,                        // reduce
        laud_binary_cross_entropy, var_binary_cross_entropy, // log loss
        laud_mse, var_mse,                                   // mse
        laud_user_elementary_fn,
        var_user_elementary_fn,                // uefn
        laud_value_at_offset, value_at_offset, //
        dtor, laud_var_dtor,                   // destructor
        NULL);
  }

  atexit(finish_lib);
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

struct laud_narray *laud_evaluate_var_node(void *node) {
  const struct laud_var_class *class = classOf(node);

  struct laud_var *var = node;
  void *result = class->evaluate_node(node);
  if (var->value && result != var->value) {
    blip(var->value);
  }

  return var->value = result;
}

static void *solve_var(struct laud_var *var) {
  if (!var->value) {
    UbjectError.error("%s (@ %p) has no value", className(var), var);
  }
  return var->value;
}

static void *differentiate_var(struct laud_var *var, uint64_t index,
                               struct laud_narray *pre_dx) {
  if (!var->value) {
    UbjectError.error("%s (@ %p) has no value", className(var), var);
  }
  return pre_dx;
}
static void *laud_var_ctor(void *self, va_list *args) {

  struct laud_var *independent_var = NULL;
  if ((independent_var = va_arg(*args, void *))) {

    // nodes do not have a means to keep track of their incoming node
    // because this information is only required during their creation
    // and never used during destructing or anywhere else so we keep
    // track of the incoming nodes here.
    uint64_t count = 0;
    uint64_t capacity = 0;

    struct laud_node *dependent_var = self;

    while (independent_var) {

      // connect nodes dependent_var and independent_var
      // dependent_var <<== independent_var
      insert_node(&((struct laud_node *)dependent_var)->incoming,
                  independent_var, &count, &capacity, 0);

      // independent_var ==>> dependent_var
      insert_node(&((struct laud_node *)independent_var)->outgoing,
                  dependent_var,
                  &((struct laud_node *)independent_var)->outgoing_count,
                  &((struct laud_node *)independent_var)->outgoing_capacity, 1);
      reference(independent_var);
      independent_var = va_arg(*args, void *);
    }
  }
  return self;
}
static void *laud_var_dtor(void *self) {
  struct laud_var *dependent_var = self;

  if (dependent_var->derivative) {
    blip(dependent_var->derivative);
  }

  if (dependent_var->value) {
    blip(dependent_var->value);
  }

  //  UbjectError.warn("destroyed %s data @ %p\n", className(self), self);
  return super_dtor(LaudVar, self);
}

static void *laud_var_class_ctor(void *self_, va_list *args) {
  struct laud_var_class *self = super_ctor(LaudVarClass, self_, args);

  typedef void (*voidf)();

  voidf selector;
  va_list arg = *args;

  while ((selector = va_arg(arg, voidf))) {

    voidf method = va_arg(arg, voidf);
    if (method) {
      if (selector == (voidf)laud_evaluate_var_node)
        memcpy(&self->evaluate_node, &method, sizeof(method));
      if (selector == (voidf)laud_differentiate_var_node)
        memcpy(&self->differentiate_node, &method, sizeof(method));
    }
  }

  return self;
}

void *laud_var() { return init(LaudVar, NULL); }

void laud_set_variable_value(void *variable_node, void *value,
                             void **old_value_ptr) {
  struct laud_var *variable = variable_node;
  struct laud_var *old_value = variable->value;

  if (old_value_ptr) {
    *old_value_ptr = old_value;
  }

  // if it is an independent node and its narray (value) can be changed
  if (classOf(variable) == LaudVar) {

    if (variable->value != value) {
      variable->value = value;
      // reference external value
      reference(value);

      if (!old_value_ptr && old_value)
        blip(old_value);
    }
  } else {

    UbjectError.warn("node_addr: %p\n"
                     "node_val:  %p\n"
                     "new_value: %p",
                     variable, variable->value, value);

    UbjectError.warn(
        "ignored request to change the n-array of dependent (%s) node @ %p",
        className(variable), variable);

    if (!old_value_ptr)
      *old_value_ptr = NULL;
  }
}

void laud_unset_variable_value(void *variable_node) {
  struct laud_var *variable = variable_node;

  blip(variable->value);
  variable->value = NULL;
}

static char *var_to_string(const void *laud_object, char *buffer,
                           uint64_t buf_limit) {
  const struct laud_var *var = laud_object;

  if (var->value) {
    return laud_to_string(var->value, buffer, buf_limit);
  } else {
    snprintf(buffer, buf_limit, "<%s @ %p>\n", className(var), var);
  }

  return buffer;
}

static void *var_reduce(const struct laud_var *operand, uint16_t axis,
                        number_t (*callback)(const number_t current_net,
                                             const number_t *const values,
                                             const void *args),
                        const void *args, void __attribute__((unused))*null) {
  struct laud_var *reduce =
      init(LaudReduce, operand, axis, callback, args, NULL);

  if (operand->value) {
    laud_evaluate_var_node(reduce);
  }

  return reduce;
}

static const uint64_t *var_shape(const struct laud_var *var) {

  if (var->value) {
    return laud_shape(var->value);
  } else {
    UbjectError.warn("ignored query for shape of a placeholder");
    return NULL;
  }
}

static uint16_t var_rank(const struct laud_var *var) {

  if (var->value) {
    return laud_rank(var->value);
  } else {
    UbjectError.warn("ignored query for rank of a placeholder");
    return 0;
  }
}

static void var_evaluate(const struct laud_var *var) {

  // list all dependencies
  struct laud_var **vars =
      (struct laud_var **)depth_first_traverse((struct laud_node *)var);

  // for each dependency call evaluate()
  struct laud_var **active_var = vars;
  while (active_var && *active_var) {

    laud_evaluate_var_node(*active_var);

    if ((*active_var)->derivative) {
      blip((*active_var)->derivative);
      (*active_var)->derivative = NULL;
    }

    ((struct laud_node *)*active_var)->is_visited = 0;
    active_var++;
  }

  FREE(vars);
}

struct laud_narray *
laud_differentiate_var_node(struct laud_var *node_var, uint64_t respect_index,
                            const struct laud_narray *derivative) {
  const struct laud_var_class *class = classOf(node_var);

  return class->differentiate_node(node_var, respect_index, derivative);
}

static void
differentiate_in_preorder_manner(struct laud_var *var,
                                 const struct laud_narray *derivative) {
  // Note: only perform this operation for LaudVar and not its children class
  if (classOf(var) == LaudVar) {

    var->derivative = derivative_add(var->value, var->derivative, derivative);
    return;
  }

  struct laud_var **independent_vars =
      (struct laud_var **)((struct laud_node *)var)->incoming;

  // preorder for each independent var
  uint64_t i = 0;
  while (*independent_vars) {
    // differentiate
    struct laud_var *active_var = *independent_vars;
    struct laud_narray *derivative_wrt_active_var =
        laud_differentiate_var_node(var, i++, derivative);
    differentiate_in_preorder_manner(active_var, derivative_wrt_active_var);

    // printf("%i \n", (int)getReference(derivative_wrt_active_var));abort();

    independent_vars++;
    blip(derivative_wrt_active_var);
  }
}

static void var_differentiate(struct laud_var *var,
                              const struct laud_narray *derivative) {
  if (!narray(var)) {
    UbjectError.error(
        "derivatives cannot be computed until the graph has been evatuated");
  }
  differentiate_in_preorder_manner(var, derivative);
}

char is_laud_var(const void *laud_object) {
  const void *class = classOf(laud_object);

  const void *laud_parent = super(LaudVar);
  while ((class)) {
    if (class == LaudVar || class == laud_parent || class == Ubject ||
        class == TypeClass) {
      return class == LaudVar;
    }
    class = super(class);
  }

  return 0;
}

void *laud_derivative_of(void *var_node) {
  if (!is_laud_var(var_node)) {
    UbjectError.error("argument is not LaudVar");
  }
  return ((struct laud_var *)var_node)->derivative;
}

void *laud_value(void *var_node) { return narray(var_node); }

static number_t value_at_offset(void *operand, uint64_t offset) {
  return laud_value_at_offset(narray(operand), offset);
}

