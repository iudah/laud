#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <Ubject.h>

#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

#define LAUD_OPERATOR_QUOTIENT_PRIORITY (LAUD_OPERATOR_PRIORITY + 2)

// Qoutient
struct LaudQuotient {
  struct LaudOperator _;
};

static void *LaudQuotient_ctor(void *self_, va_list *args) {
  // Cast self_ to LaudQuotient type
  struct LaudQuotient *self = init(LaudQuotient, self_, args);

  // Extract operands a and b from the variable arguments
  void *operand_a = va_arg(*args, struct LaudVar *);
  void *operand_b = va_arg(*args, struct LaudVar *);

  // Reserve space for operands in the operator stack
  LaudOperatorProtected.reserve(self, 2);

  // Push operands onto the operator stack
  LaudOperatorProtected.push(self, operand_a);
  LaudOperatorProtected.push(self, operand_b);

  // Return the initialized LaudQuotient object
  return self;
}

static void *LaudQuotient_eval(void *self) {

  // Get the length of the LaudQuotient object
  size_t this_length = laud_length(self);

  // Get dependencies from the operator stack
  const struct LaudStack *dependencies = LaudOperatorProtected.dependency(self);

  LaudStackFn.iter_start((struct LaudStack *)dependencies);
  const float *const numerator_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  const float *const denominator_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  LaudStackFn.iter_end((struct LaudStack *)dependencies);

  // Get values from the LaudQuotient object
  float *this_values = (float *)laud_values(self);

  // Calculate the quotient element-wise
  for (size_t i = 0; i < this_length;
       i++) { // Check for division by zero before performing the division
    if (denominator_values[i] != 0.0) {
      this_values[i] = numerator_values[i] / denominator_values[i];
    } else {
      // Handle division by zero (e.g., set result to NaN or throw an error)
      // TODO: Use NaN or throw an error
      UbjectError.warn("LaudQuotient_eval: division by zero");
      this_values[i] = 0.0; // Replace with better handling
    }
  }

  // Return the LaudQuotient object
  return self;
}

static void LaudOperator_computeDerivative(const void *self_,
                                           const void *d_var_d_self,
                                           struct LaudQueue *derivatives) {
  printf("qoutient:");
  void *x1 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 0);
  void *x2 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 1);

  if (laud_is_continuous(x1)) {
    LaudQueueFn.enqueue(derivatives, x1);
    LaudQueueFn.enqueue(derivatives, laud_quotient(d_var_d_self, x2)); //???
  }

  if (laud_is_continuous(x2)) {
    LaudQueueFn.enqueue(derivatives, x2);
    // Todo: improve implementation
    //  dy/dx = (v * du/dx - u * dv/dx) / v^2
    LaudQueueFn.enqueue(derivatives,
                        LaudOperatorProtected.update_respect_product(
                            self_, x1, d_var_d_self)); //???
  }
  UbjectError.warn("Faulty implementation");
}

// initialize library section
const void *LaudQuotient = NULL;
void __attribute__((constructor(LAUD_OPERATOR_QUOTIENT_PRIORITY)))
initLaudQuotient(void) {

  if (!LaudQuotient) {
    LaudQuotient =
        init(LaudOperatorClass, LaudOperator,
             sizeof(struct LaudQuotient), // class, parent, size
             ctor, LaudQuotient_ctor, className, "LaudQuotient", laud_evaluate,
             LaudQuotient_eval, LaudOperatorProtected.compute_derivative,
             LaudOperator_computeDerivative, NULL);
  }
}
