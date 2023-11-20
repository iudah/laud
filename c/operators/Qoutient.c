#include <stdarg.h>
#include <stddef.h>

#include "../Const.h"
#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

// Qoutient
struct LaudQoutient {
  struct LaudOperator _;
};
static void *LaudQoutient_ctor(void *self_, va_list *args) {
  struct LaudQoutient *self = self_;

  LaudOperatorProtected.reserve(self, 2);
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));

  LaudVarProtected.setValue(
      self, LaudVarProtected.getValue(
                LaudStackFn.peek(LaudOperatorProtected.dependency(self), 0)) /
                LaudVarProtected.getValue(LaudStackFn.peek(
                    LaudOperatorProtected.dependency(self), 0)));
  return self;
}

static void *LaudQoutient_eval(void *self_) {
  printf("qoutient\n");
  return self_;
}
static const void *
LaudOperator_computeDerivative(const void *self_, const void *d_var_d_self,
                               struct LaudQueue *derivatives) {
  printf("qoutient:");
  void *x1 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 0);
  void *x2 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 1);
  if (d_var_d_self) {
    if (LaudVarIsContinous(x1)) {
      LaudQueueFn.enqueue(derivatives, x1);
      LaudQueueFn.enqueue(derivatives, LaudVarQoutient(d_var_d_self, x2));
    }
    if (LaudVarIsContinous(x2)) {
      LaudQueueFn.enqueue(derivatives, x2);
      LaudQueueFn.enqueue(
          derivatives,
          LaudOperatorProtected.update_respect_product(x1, d_var_d_self));
    }
  } else {
    if (LaudVarIsContinous(x1)) {
      LaudQueueFn.enqueue(derivatives, x1);
      LaudQueueFn.enqueue(derivatives, LaudVarQoutient(One, x2));
    }
    if (LaudVarIsContinous(x2)) {
      LaudQueueFn.enqueue(derivatives, x2);
      // not multiplying? not creating a new object? use as is? then reference
      reference(x1);
      LaudQueueFn.enqueue(derivatives, x1);
    }
  }
  return self_;
}

// initialize library section
const void *LaudQoutient = NULL;
void __attribute__((constructor(25))) initLaudQoutient(void) {

  if (!LaudQoutient)
    LaudQoutient = init(LaudOperatorClass, LaudOperator,
                        sizeof(struct LaudQoutient), // class, parent, size
                        ctor, LaudQoutient_ctor, className, "LaudQoutient",
                        LaudVarEvaluate, LaudQoutient_eval,
                        LaudOperatorProtected.ComputeDerivative,
                        LaudOperator_computeDerivative, NULL);
}
