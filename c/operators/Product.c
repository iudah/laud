#include <stdarg.h>
#include <stddef.h>

#include <Ubject.h>

#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

#define LAUD_OPERATOR_PRODUCT_PRIORITY (LAUD_OPERATOR_PRIORITY + 2)

// Product
struct LaudProduct {
  struct LaudOperator _;
};
static void *LaudProduct_ctor(void *instance, va_list *args) {
  struct LaudProduct *product_instance = init(LaudProduct, instance, args);

  void *operand_a, *operand_b;
  LaudOperatorProtected.reserve(instance, 2);
  LaudOperatorProtected.push(instance,
                             operand_a = va_arg(*args, struct LaudVar *));
  LaudOperatorProtected.push(instance,
                             operand_b = va_arg(*args, struct LaudVar *));

  return product_instance;
}

static void *LaudProduct_eval(void *instance) {

  // Get the length of the current LaudProduct instance
  size_t instance_length = laud_length(instance);

  // Get dependencies from the operator
  const struct LaudStack *dependencies =
      LaudOperatorProtected.dependency(instance);

  // Get the values of the operands from the dependencies
  LaudStackFn.iter_start((struct LaudStack *)dependencies);
  const float *const operand_a_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  const float *const operand_b_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  LaudStackFn.iter_end((struct LaudStack *)dependencies);

  // Get the values of the current LaudProduct instance
  float *instance_values = (float *)laud_values(instance);

  // Perform element-wise multiplication
  for (size_t i = 0; i < instance_length; i++) {
    instance_values[i] = operand_a_values[i] * operand_b_values[i];
  }

  return instance;
}

static void LaudOperator_computeDerivative(const void *instance,
                                           const void *d_var_d_instance,
                                           struct LaudQueue *derivatives) {
  // Get the dependencies of the LaudProduct instance
  void *operand_a =
      LaudStackFn.peek(LaudOperatorProtected.dependency(instance), 0);
  void *operand_b =
      LaudStackFn.peek(LaudOperatorProtected.dependency(instance), 1);

  // Check if the first operand is continuous
  if (laud_is_continuous(operand_a)) {
    // Enqueue the first operand into the derivatives queue
    LaudQueueFn.enqueue(derivatives, operand_a);

    // Compute the derivative with respect to the first operand and enqueue it
    LaudQueueFn.enqueue(derivatives,
                        LaudOperatorProtected.update_respect_product(
                            instance, operand_b, d_var_d_instance));
  }

  // Check if the second operand is continuous
  if (laud_is_continuous(operand_b)) {
    // Enqueue the second operand into the derivatives queue
    LaudQueueFn.enqueue(derivatives, operand_b);

    // Compute the derivative with respect to the second operand and enqueue it
    LaudQueueFn.enqueue(derivatives,
                        LaudOperatorProtected.update_respect_product(
                            instance, operand_a, d_var_d_instance));
  }
}

// Global variable to store the LaudProduct class instance
const void *LaudProduct = NULL;

// Constructor function for initializing LaudProduct class
void __attribute__((constructor(LAUD_OPERATOR_PRODUCT_PRIORITY)))
initLaudProduct(void) {
  // Check if LaudProduct is not already initialized
  if (!LaudProduct) {
    // Initialize LaudProduct as a LaudOperatorClass
    LaudProduct = init(LaudOperatorClass, LaudOperator,
                       sizeof(struct LaudProduct),      // class, parent, size
                       ctor, LaudProduct_ctor,          // Constructor function
                       className, "LaudProduct",        // Class name
                       laud_evaluate, LaudProduct_eval, // Evaluation functions
                       LaudOperatorProtected.compute_derivative,
                       LaudOperator_computeDerivative, // Derivative function
                       NULL);
  }
}
