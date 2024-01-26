#include <stdarg.h>
#include <stddef.h>

#include <Ubject.h>

#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

#define LAUD_OPERATOR_MATRIXDOT_PRIORITY (LAUD_OPERATOR_PRIORITY + 2)

// MatrixDot
struct LaudMatrixDot {
  struct LaudOperator _;
};

static void *LaudMatrixDot_ctor(void *instance, va_list *args) {
  struct LaudMatrixDot *self = super_ctor(LaudMatrixDot, instance, args);

  // Extract matrix operands from arguments
  void *matrix_a = va_arg(*args, struct LaudVar *);
  void *matrix_b = va_arg(*args, struct LaudVar *);

  // Reserve space and push matrix operands to the dependency stack
  LaudOperatorProtected.reserve(self, 2);
  LaudOperatorProtected.push(self, matrix_a);
  LaudOperatorProtected.push(self, matrix_b);

  return self;
}

static void *LaudMatrixDot_eval(void *self) {
  printf("matrixdot\n");
  size_t this_length = laud_length(self);

  const struct LaudStack *dependencies = LaudOperatorProtected.dependency(self);

  float *this_values = (float *)laud_values(self);

  LaudStackFn.iter_start((struct LaudStack *)dependencies);
  const float *const a_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  const float *const b_values =
      laud_values(LaudStackFn.yield((struct LaudStack *)dependencies));
  LaudStackFn.iter_end((struct LaudStack *)dependencies);

  // todo: fix implementation of matrix dot
  for (size_t i = 0; i < this_length; i++) {
    this_values[i] = a_values[i] * b_values[i];
  }

  return self;
}

static  void
LaudOperator_computeDerivative(const void *self_, const void *d_var_d_self,
                               struct LaudQueue *derivatives) {
  const struct LaudStack *dependency_stack =
      LaudOperatorProtected.dependency(self_);
  void *x1 = LaudStackFn.peek(dependency_stack, 0);
  void *x2 = LaudStackFn.peek(dependency_stack, 1);

  if (laud_is_continuous(x1)) {
    LaudQueueFn.enqueue(derivatives, x1);
    LaudQueueFn.enqueue(
        derivatives,
        LaudOperatorProtected.update_respect_product(self_, x2, d_var_d_self));
  }

  if (laud_is_continuous(x2)) {
    LaudQueueFn.enqueue(derivatives, x2);
    LaudQueueFn.enqueue(
        derivatives,
        LaudOperatorProtected.update_respect_product(self_, x1, d_var_d_self));
  }
}
// initialize library section
const void *LaudMatrixDot = NULL;
void __attribute__((constructor(LAUD_OPERATOR_MATRIXDOT_PRIORITY)))
initLaudMatrixDot(void) {
  if (!LaudMatrixDot) {
    LaudMatrixDot = init(LaudOperatorClass, LaudOperator,
                         sizeof(struct LaudMatrixDot), // class, parent, size
                         ctor, LaudMatrixDot_ctor, className, "LaudMatrixDot",
                         laud_evaluate, LaudMatrixDot_eval,
                         LaudOperatorProtected.compute_derivative,
                         LaudOperator_computeDerivative, NULL);
  }
}
