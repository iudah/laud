#include <stdarg.h>
#include <stddef.h>

#include <Ubject.h>

#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

#define LAUD_OPERATOR_ADD_PRIORITY (LAUD_OPERATOR_PRIORITY + 2)

// Addition
struct LaudAdd {
  struct LaudOperator _;
};

static void *laud_add_ctor(void *instance, va_list *args) {
  struct LaudAdd *add_instance = super_ctor(LaudAdd, instance, args);

  void *operand_a = va_arg(*args, struct LaudVar *);
  void *operand_b = va_arg(*args, struct LaudVar *);
  LaudOperatorProtected.reserve(add_instance, 2);
  LaudOperatorProtected.push(add_instance, operand_a);
  LaudOperatorProtected.push(add_instance, operand_b);

  return add_instance;
}

static void *LaudAdd_evaluate(void *add_instance) {

  size_t result_length = laud_length(add_instance);

  // Get dependencies stack
  const struct LaudStack *dependencies =
      LaudOperatorProtected.dependency(add_instance);

  // Get values of operands from the dependencies stack
  LaudStackFn.iter_start((struct LaudStack *)dependencies);
  const float *const operand_a_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  const float *const operand_b_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  LaudStackFn.iter_end((struct LaudStack *)dependencies);

  // Get values of the result
  float *result_values = (float *)laud_values(add_instance);

  // Perform addition element-wise
  for (size_t i = 0; i < result_length; i++) {
    result_values[i] = operand_a_values[i] + operand_b_values[i];
  }

  return add_instance;
}

static void laud_operator_compute_derivative(const void *operator_instance,
                                             const void *d_variable_d_operator,
                                             struct LaudQueue *derivatives) {
  void *operand_a =
      LaudStackFn.peek(LaudOperatorProtected.dependency(operator_instance), 0);
  void *operand_b =
      LaudStackFn.peek(LaudOperatorProtected.dependency(operator_instance), 1);

  // Enqueue derivatives for continuous operands
  if (laud_is_continuous(operand_a)) {
    LaudQueueFn.enqueue(derivatives, operand_a);
    // not multiplying? not creating a new object? use as is? then reference
    reference((void *)d_variable_d_operator);
    LaudQueueFn.enqueue(derivatives, d_variable_d_operator);
  }

  if (laud_is_continuous(operand_b)) {
    LaudQueueFn.enqueue(derivatives, operand_b);
    // not multiplying? not creating a new object? use as is? then reference
    reference((void *)d_variable_d_operator);
    LaudQueueFn.enqueue(derivatives, d_variable_d_operator);
  }
}

const void *LaudAdd = NULL;

void __attribute__((constructor(LAUD_OPERATOR_ADD_PRIORITY)))
initLaudAdd(void) {
  // Check if LaudAdd is already initialized to avoid redundant initialization
  if (!LaudAdd)
    // Initialize LaudAdd using the init function
    LaudAdd = init(LaudOperatorClass, LaudOperator,
                   sizeof(struct LaudAdd),          // class, parent, size
                   ctor, laud_add_ctor,             // constructor function
                   className, "LaudAdd",            // class name
                   laud_evaluate, LaudAdd_evaluate, // evaluation functions
                   LaudOperatorProtected.compute_derivative,
                   laud_operator_compute_derivative, // derivative function
                   NULL);
}
