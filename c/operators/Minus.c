#include <stdarg.h>
#include <stddef.h>

#include <Ubject.h>

#include "../Const.h"
#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

#define LAUD_OPERATOR_MINUS_PRIORITY (LAUD_OPERATOR_PRIORITY + 2)

struct LaudMinus {
  struct LaudOperator _;
};

static void *LaudMinus_ctor(void *instance, va_list *args) {
  struct LaudMinus *self = init(LaudMinus, instance, args);

  // Declare variables for operands
  void *operand_a = va_arg(*args, struct LaudVar *),
       *operand_b = va_arg(*args, struct LaudVar *);

  LaudOperatorProtected.reserve(self, 2);
  LaudOperatorProtected.push(self, operand_a);
  LaudOperatorProtected.push(self, operand_b);

  return self;
}

static void *LaudMinus_eval(void *instance) {
  // Get the length of the current instance
  size_t instance_length = laud_length(instance);

  // Get dependencies from the operator's protected data
  const struct LaudStack *dependencies =
      LaudOperatorProtected.dependency(instance);

  float *instance_values = (float *)laud_values(instance);

  LaudStackFn.iter_start((struct LaudStack *)dependencies);
  const float *const operand_a_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  const float *const b_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  LaudStackFn.iter_end((struct LaudStack *)dependencies);

  for (size_t i = 0; i < instance_length; i++) {
    instance_values[i] = operand_a_values[i] - b_values[i];
  }

  return instance;
}

static void LaudOperator_computeDerivative(const void *instance,
                                           const void *d_variable_d_instance,
                                           struct LaudQueue *derivatives) {
  // Peek the dependencies
  void *operand_a =
      LaudStackFn.peek(LaudOperatorProtected.dependency(instance), 0);
  void *operand_b =
      LaudStackFn.peek(LaudOperatorProtected.dependency(instance), 1);

  if (laud_is_continuous(operand_a)) {
    LaudQueueFn.enqueue(derivatives, operand_a);
    // not multiplying? not creating a new object? use as is? then reference
    reference((void *)d_variable_d_instance);
    LaudQueueFn.enqueue(derivatives, d_variable_d_instance);
  }

  if (laud_is_continuous(operand_b)) {
    LaudQueueFn.enqueue(derivatives, operand_b);
    LaudQueueFn.enqueue(derivatives,
                        LaudOperatorProtected.update_respect_product(
                            instance, MinusOne, d_variable_d_instance));
  }
}

// initialize library section
const void *LaudMinus = NULL;
void __attribute__((constructor(LAUD_OPERATOR_MINUS_PRIORITY)))
initLaudMinus(void) {

  if (!LaudMinus)
    LaudMinus =
        init(LaudOperatorClass, LaudOperator,
             sizeof(struct LaudMinus), // class, parent, size
             ctor, LaudMinus_ctor, className, "LaudMinus", laud_evaluate,
             LaudMinus_eval, LaudOperatorProtected.compute_derivative,
             LaudOperator_computeDerivative, NULL);
}
