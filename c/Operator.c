#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <Ubject.h>

#include "Const.h"
#include "ds/HashMap.h"
#include "ds/Queue.h"
#include "ds/Stack.h"

#define OPERATOR_C
#include "Operator.r.h"
#include "Var.h"
#include "Var.r.h"

static void *laud_operator_constructor(void *self /*, va_list *args*/) {
  return self;
}
static void *LaudOperator_dtor(void *self) {
  struct LaudOperator *this = self;

  // Remove dependencies
  void *node;
  while ((node = LaudStackFn.pop(this->dependency))) {
    blip(node);
  }

  LaudStackFn.del(this->dependency);

  // Call superclass destructor
  super_dtor(LaudOperator, this);

  return this;
}
static const struct LaudStack *
laud_operator_dependency(const void *laud_operator) {
  return ((const struct LaudStack *)((struct LaudOperator *)(laud_operator))
              ->dependency);
}

static int laud_operator_reserve_dependency_array(void *laud_operator,
                                                  int count) {
  struct LaudOperator *self = laud_operator;
  self->dependency = LaudStackFn.Stack(count);
  return count;
}

static void *laud_operator_push_dependency(void *self, void *dependency) {
  // Cast to LaudOperator type
  struct LaudOperator *laud_operator = self;

  // Get the dependency stack from the LaudOperator
  struct LaudStack *dependency_stack = laud_operator->dependency;

  // Push the dependency onto the stack
  LaudStackFn.push(dependency_stack, dependency);

  // Check continuity conditions and update if necessary
  if (!LaudStackFn.count(dependency_stack) ||
      (!laud_is_continuous(laud_operator) &&
       (dependency == LAUD_PLACEHOLDER ? 1 : laud_is_continuous(dependency)))) {
    laud_set_continuity(laud_operator, 1);
  }

  // Increment reference count for the dependency
  reference(dependency);

  // Return the pushed dependency
  return dependency;
}

static char is_operator(const void *self) {
  const void *class = classOf(self);

  while (class) {
    // Check if the current class is LaudOperator or its subclasses
    if (class == LaudOperator || class == LaudVar || class == LaudConst ||
        class == Ubject || class == TypeClass) {
      return class == LaudOperator;
    }

    // Move up the class hierarchy
    class = super(class);
  }

  // Handle the case where classOf(self) is NULL
  return 0;
}

static void *multiply_respects(const void *x, const void *a, const void *b) {
  if (!x || !a || !b) {
    // Handle NULL inputs
    UbjectError.error("Error: NULL input in multiply_respects.\n");
    return NULL;
  }

  struct LaudVar *product = laud_const(laud_rank(x), laud_shape(x), 0, NULL);

  if (!product) {
    // Handle memory allocation failure
    UbjectError.error(
        "Error: Memory allocation failed in multiply_respects.\n");
    return NULL;
  }

  float *product_values = (float *)laud_values(product);
  const float *a_values = laud_values(a);
  const float *b_values = laud_values(b);

  size_t length = laud_length(x);
  for (size_t i = 0; i < length; ++i) {
    product_values[i] = a_values[i] * b_values[i];
  }

  return product;
}

static void compute_operator_derivative(const void *operator_instance,
                                        const void *derivative_of_top_variable,
                                        struct LaudQueue *result_derivatives) {
  const struct LaudOperatorClass *operator_class = classOf(operator_instance);

  if (!operator_class) {
    UbjectError.error("compute_operator_derivative: Object class not found\n");
  } else if (!operator_class->compute_derivative) {
    UbjectError.error("%s: compute_derivative function missing\n",
                      className(operator_instance));
  }

  return operator_class->compute_derivative(
      operator_instance, derivative_of_top_variable, result_derivatives);
}

const struct LaudOperatorProtected LaudOperatorProtected = {
    .dependency = laud_operator_dependency,
    .reserve = laud_operator_reserve_dependency_array,
    .push = laud_operator_push_dependency,
    .is_operator = is_operator,
    .update_respect_product = multiply_respects,
    .compute_derivative = compute_operator_derivative};

static inline void sum_derivatives(float *result, const float *operand_a,
                                   const float *operand_b,
                                   const size_t array_length) {
  for (size_t i = 0; i < array_length; i++) {
    result[i] = operand_a[i] + operand_b[i];
  }
}

static void update_derivative(struct LaudHashMap *all_derivatives,
                              struct LaudVar *x,
                              void *current_step_derivatives) {
  int hash;
  const void *current_net_derivatives =
      LaudHashMapFn.find(all_derivatives, x, &hash);

  if (!current_net_derivatives) {
    // If no existing derivative, insert the current one
    LaudHashMapFn.insert(all_derivatives, x, current_step_derivatives);
  } else {
    if (getReference((void *)current_net_derivatives) == 0) {
      // If reference count is zero, update the existing derivative in place
      float *dydx_values = (float *)laud_values(current_net_derivatives);
      sum_derivatives(dydx_values, dydx_values,
                      laud_values(current_step_derivatives),
                      laud_length(current_net_derivatives));
    } else {
      // If reference count is not zero, create a new derivative
      void *new_net_derivatives =
          laud_const(laud_rank(current_net_derivatives),
                     laud_shape(current_net_derivatives), 0, NULL);

      // Replace the existing derivative with the new one
      LaudHashMapFn.replace_key_using_hash(all_derivatives, x,
                                           new_net_derivatives, hash);

      // Sum the values of the existing and current derivatives
      sum_derivatives((float *)laud_values(new_net_derivatives),
                      laud_values(current_net_derivatives),
                      laud_values(current_step_derivatives),
                      laud_length(new_net_derivatives));

      // Decrement the reference count of the existing derivative
      blip((void *)current_net_derivatives);
    }
  }
}

// must be final
static void
differentiate_operator(const void *operator_instance,
                       const void *const derivative_of_top_var_wrt_self_,
                       struct LaudHashMap *ddx) {
  if (laud_is_continuous(operator_instance)) {
    if (is_operator(operator_instance)) {
      const struct LaudOperator *operator= operator_instance;

      const void *const derivative_of_top_var_wrt_self =
          derivative_of_top_var_wrt_self_ ? derivative_of_top_var_wrt_self_
                                          : One;
      // List of derivatives
      struct LaudQueue *derivatives =
          LaudQueueFn.Queue(LaudStackFn.count(operator->dependency));

      // Enqueue self and its derivative
      LaudQueueFn.enqueue(derivatives, operator);
      reference((void *)derivative_of_top_var_wrt_self);
      LaudQueueFn.enqueue(derivatives, derivative_of_top_var_wrt_self);

      // For each item in self.dependency
      const void *current_variable;

      while ((current_variable = LaudQueueFn.dequeue(derivatives))) {
        const void *derivative_of_topmost_variable_wrt_current_variable =
            LaudQueueFn.dequeue(derivatives);

        if (is_operator(current_variable)) {
          // If the current_variable is an operator, compute its derivative
          compute_operator_derivative(
              current_variable,
              derivative_of_topmost_variable_wrt_current_variable, derivatives);
        } else {
          // If the current_variable is not an operator, update the derivative
          if (laud_is_continuous(current_variable)) {
            printf("var: ");
            update_derivative(
                ddx, (void *)current_variable,
                (void *)derivative_of_topmost_variable_wrt_current_variable);

          } else
            printf("const: ");
        }
      }
    }
  }
}

static void *LaudOperatorClass_ctor(void *instance, va_list *args) {
  struct LaudOperatorClass *class_instance =
      super_ctor(LaudOperatorClass, instance, args);
  typedef void (*voidf)();
  voidf selector;
  va_list arg = *args;
  while ((selector = va_arg(arg, voidf))) {
    voidf method = va_arg(arg, voidf);
    if (selector == (voidf)compute_operator_derivative)
      *(voidf *)&class_instance->compute_derivative = method;
  }
  return class_instance;
}

const void *LaudOperatorClass = NULL;
const void *LaudOperator = NULL;

static void __attribute__((constructor(LAUD_OPERATOR_PRIORITY)))
initialize_laud_operator(void) {
  if (!LaudOperatorClass) {
    LaudOperatorClass =
        init(TypeClass, LaudVarClass, sizeof(struct LaudOperatorClass), ctor,
             LaudOperatorClass_ctor, NULL);
  }

  if (!LaudOperator) {
    LaudOperator = init(LaudOperatorClass, LaudVar,
                        sizeof(struct LaudOperator), // class, parent, size
                        ctor, laud_operator_constructor, className,
                        "LaudOperator", dtor, LaudOperator_dtor,
                        laud_differentiate, differentiate_operator, NULL);
  }
}

void *__attribute__((warn_unused_result))
laud_change_dependency(void *operator_instance, void *new_dependency,
                       size_t index) {

  struct LaudOperator *operator_object = operator_instance;

  return LaudStackFn.replace_item(operator_object->dependency, new_dependency,
                                  index);
}
