#include <Ubject.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c/core/base.h"
#include "var.h"
#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../math/add.r.h"
#include "../math/matrix_dot.r.h"
#include "../math/nn_activations.h"
#include "../math/relu.r.h"
#include "../math/sigmoid.r.h"
#include "../misc/slice.r.h"
#include "var.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_var(struct laud_var *var);

static void *laud_var_ctor(void *self, va_list *args);

static void *laud_var_class_ctor(void *self_, va_list *args);

static char *var_to_string(const void *laud_object, char *buffer,
                           size_t buf_limit);

static void *var_slice(const struct laud_var *operand, const char *slice_fmt);

static void *var_matrix_dot(const struct laud_var *operand_a,
                            const struct laud_var *operand_b);

static void *var_add(const struct laud_var *operand_a,
                     const struct laud_var *operand_b);

static void *var_relu(const struct laud_var *operand_a);

static void *var_sigmoid(const struct laud_var *operand_a);

static const size_t *var_shape(const struct laud_var *var);

static size_t var_rank(const struct laud_var *var);

static void var_evaluate(const struct laud_var *var);

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

static void __attribute__((constructor(LAUD_VAR_PRIORITY)))
library_initializer(void) {
  if (!LaudVarClass) {
    LaudVarClass = init(TypeClass, LaudNodeClass, sizeof(struct laud_var_class),
                        ctor, laud_var_class_ctor, NULL);
  }
  if (!LaudVar) {
    LaudVar = init(LaudVarClass, LaudNode,
                   sizeof(struct laud_var),         // class parent size
                   className, "LaudVar",            // class name
                   evaluate_node, solve_var,        // evaluate_node
                   ctor, laud_var_ctor,             // constructor
                   laud_to_string, var_to_string,   // to string
                   laud_slice, var_slice,           // slice
                   laud_matrix_dot, var_matrix_dot, // matrix dot
                   laud_add, var_add,               // addition
                   laud_relu, var_relu,             // relu
                   laud_sigmoid, var_sigmoid,       // sigmoid
                   laud_shape, var_shape,           // shape
                   laud_rank, var_rank,             // rank
                   laud_evaluate, var_evaluate,     // evaluate
                   //   dtor, laud_var_dtor, // destructor
                   NULL);
  }
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

static void *solve_var(struct laud_var *var) {
  if (!var->value) {
    UbjectError.error("laud_var (@ %p) has no value", var);
  }
  return var->value;
}

static inline void insert_incoming_node(struct laud_node *node,
                                        struct laud_node *incoming_node,
                                        size_t *count, size_t *capacity) {
  printf("%p <= %p\n", node, incoming_node);
  return insert_node(&node->incoming, incoming_node, count, capacity);
}

static inline void insert_outgoing_node(struct laud_node *node,
                                        struct laud_node *outgoing_node) {
  printf("%p <= %p\n", node, outgoing_node);
  return insert_node(&node->outgoing, outgoing_node, &node->outgoing_count,
                     &node->outgoing_capacity);
}

static void *laud_var_ctor(void *self, va_list *args) {
  size_t count = 0;
  size_t capacity = 0;

  struct laud_node *independent_var = NULL;
  while ((independent_var = va_arg(*args, void *))) {

    insert_incoming_node(self, independent_var, &count, &capacity);
    insert_outgoing_node(independent_var, self);
  }

  return self;
}

static void *laud_var_class_ctor(void *self_, va_list *args) {
  struct laud_var_class *self = super_ctor(LaudVarClass, self_, args);

  typedef void (*voidf)();

  voidf selector;
  va_list arg = *args;

  while ((selector = va_arg(arg, voidf))) {

    voidf method = va_arg(arg, voidf);
    if (method) {
      if (selector == (voidf)evaluate_node)
        *(voidf *)&self->evaluate_node = method;
    }
  }

  return self;
}

// static void *laud_var_dtor(void *self) {
// return super_dtor(LaudVar, self);
// }

void *laud_var() { return init(LaudVar, NULL); }

void *laud_set_variable_value(void *variable_node, void *value) {
  struct laud_var *variable = variable_node;

  UbjectError.warn("%p %p %p", variable, variable->value, value);

  void *old_value;

  // if there are no incoming nodes then it is an independent node and its
  // narray can be changed
  if (!incoming_nodes(variable)) {

    old_value = variable->value;

    variable->value = value;
    reference(variable->value);

    if (old_value) {
      blip(variable->value);
    }

  } else {
    UbjectError.warn(
        "ignored request to change the n-array of dependent (%s) node @ %p",
        className(variable), variable);

    old_value = NULL;
  }
  return old_value;
}

static char *var_to_string(const void *laud_object, char *buffer,
                           size_t buf_limit) {
  const struct laud_var *var = laud_object;

  if (var->value) {
    return laud_to_string(var->value, buffer, buf_limit);
  } else {
    snprintf(buffer, buf_limit, "<%s @ %p>\n", className(var), var);
  }

  return buffer;
}

static void *var_slice(const struct laud_var *operand, const char *slice_fmt) {

  struct laud_var *slice = init(LaudSlice, operand, NULL, slice_fmt, NULL);

  if (operand->value) {
    evaluate_node(slice);
  }

  return slice;
}

static void *var_matrix_dot(const struct laud_var *operand_a,
                            const struct laud_var *operand_b) {

  struct laud_var *dot_product =
      init(LaudMatrixDot, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    evaluate_node(dot_product);
  }

  return dot_product;
}

static void *var_add(const struct laud_var *operand_a,
                     const struct laud_var *operand_b) {

  struct laud_var *addition = init(LaudAdd, operand_a, operand_b, NULL);

  if (operand_a->value && operand_b->value) {
    evaluate_node(addition);
  }

  return addition;
}

static void *var_relu(const struct laud_var *operand_a) {

  struct laud_var *relu = init(LaudReLU, operand_a, NULL);

  if (operand_a->value) {
    evaluate_node(relu);
  }

  return relu;
}

static void *var_sigmoid(const struct laud_var *operand_a) {

  struct laud_var *sigmoid = init(LaudSigmoid, operand_a, NULL);

  if (operand_a->value) {
    evaluate_node(sigmoid);
  }

  return sigmoid;
}

static const size_t *var_shape(const struct laud_var *var) {

  if (var->value) {
    return laud_shape(var->value);
  } else {
    UbjectError.warn("ignored query for shape of a placeholder");
    return NULL;
  }
}

static size_t var_rank(const struct laud_var *var) {

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

    UbjectError.warn("- - - - %p %s", *active_var, className(*active_var));

    evaluate_node(*active_var);

    ((struct laud_node *)*active_var)->is_visited = 0;
    active_var++;
  }

  free(vars);
}
