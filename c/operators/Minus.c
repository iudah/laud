#include <stdarg.h>
#include <stddef.h>

#include "../Const.h"
#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"

struct LaudMinus {
  struct LaudOperator _;
};
static void *LaudMinus_ctor(void *self_, va_list *args) {
  struct LaudMinus *self = self_;

  LaudOperatorProtected.reserve(self, 2);
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));

  LaudVarProtected.setValue(
      self, LaudVarProtected.getValue(
                LaudStackFn.peek(LaudOperatorProtected.dependency(self), 0)) -
                LaudVarProtected.getValue(LaudStackFn.peek(
                    LaudOperatorProtected.dependency(self), 0)));
  return self;
}

static void *LaudMinus_eval(void *self_) {
  printf("minus\n");
  return self_;
}

static const void *
LaudOperator_computeDerivative(const void *self_, const void *d_var_d_self,
                               struct LaudQueue *derivatives) {
  printf("minus: ");
  void *x1 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 0);
  void *x2 = LaudStackFn.peek(LaudOperatorProtected.dependency(self_), 1);
  if (d_var_d_self) {
    if (LaudVarIsContinous(x1)) {
      LaudQueueFn.enqueue(derivatives, x1);
      // not multiplying? not creating a new object? use as is? then reference
      reference((void *)d_var_d_self);
      LaudQueueFn.enqueue(derivatives, d_var_d_self);
    }
    if (LaudVarIsContinous(x2)) {
      LaudQueueFn.enqueue(derivatives, x2);
      LaudQueueFn.enqueue(
          derivatives,
          LaudOperatorProtected.update_respect_product(MinusOne, d_var_d_self));
    }
  } else {
    if (LaudVarIsContinous(x1)) {
      LaudQueueFn.enqueue(derivatives, x1);
      // not multiplying? not creating a new object? use as is? then reference
      reference((void *)One);
      LaudQueueFn.enqueue(derivatives, One);
    }
    if (LaudVarIsContinous(x2)) {
      LaudQueueFn.enqueue(derivatives, x2);
      // not multiplying? not creating a new object? use as is? then reference
      reference((void *)MinusOne);
      LaudQueueFn.enqueue(derivatives, MinusOne);
    }
  }
  return self_;
}

// initialize library section
const void *LaudMinus = NULL;
void __attribute__((constructor(25))) initLaudMinus(void) {

  if (!LaudMinus)
    LaudMinus =
        init(LaudOperatorClass, LaudOperator,
             sizeof(struct LaudMinus), // class, parent, size
             ctor, LaudMinus_ctor, className, "LaudMinus", LaudVarEvaluate,
             LaudMinus_eval, LaudOperatorProtected.ComputeDerivative,
             LaudOperator_computeDerivative, NULL);
}
