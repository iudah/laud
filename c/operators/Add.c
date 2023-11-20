#include <stdarg.h>
#include <stddef.h>

#include "../Const.h"
#include "../Operator.r.h"
#include "../Var.h"
#include "../ds/Queue.h"
// Addition
struct LaudAdd {
  struct LaudOperator _;
};
static void *LaudAdd_ctor(void *self_, va_list *args) {
  struct LaudAdd *self = self_;

  LaudOperatorProtected.reserve(self, 2);
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));
  LaudOperatorProtected.push(self, va_arg(*args, struct LaudVar *));

  LaudVarProtected.setValue(
      self, LaudVarProtected.getValue(
                LaudStackFn.peek(LaudOperatorProtected.dependency(self), 0)) +
                LaudVarProtected.getValue(LaudStackFn.peek(
                    LaudOperatorProtected.dependency(self), 0)));
  return self;
}

static void *LaudAdd_eval(void *self_) {
  printf("add\n");
  return self_;
}

static const void *
LaudOperator_computeDerivative(const void *self_, const void *d_var_d_self,
                               struct LaudQueue *derivatives) {
  printf("add: ");
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
      // not multiplying? not creating a new object? use as is? then reference
      reference((void *)d_var_d_self);
      LaudQueueFn.enqueue(derivatives, d_var_d_self);
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
      reference((void *)One);
      LaudQueueFn.enqueue(derivatives, One);
    }
  }
  return self_;
}

const void *LaudAdd = NULL;

void __attribute__((constructor(25))) initLaudAdd(void) {
  if (!LaudAdd)
    LaudAdd = init(LaudOperatorClass, LaudOperator,
                   sizeof(struct LaudAdd), // class, parent, size
                   ctor, LaudAdd_ctor, className, "LaudAdd", LaudVarEvaluate,
                   LaudAdd_eval, LaudOperatorProtected.ComputeDerivative,
                   LaudOperator_computeDerivative, NULL);
}