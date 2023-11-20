#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "../../Ubject/Ubject.h"
#include "Const.h"
#include "ds/HashMap.h"

#define OPERATOR_C
#include "Operator.r.h"
#include "Var.h"
#include "Var.r.h"

static void *LaudOperator_ctor(void *self_, va_list *args) { return self_; }
static void *LaudOperator_dtor(void *self_) {
  struct LaudOperator *self = self_;
  // remove dependencies
  void *node;
  while ((node = LaudStackFn.pop(self->dependency))) {
    blip(node);
  }

  LaudStackFn.del(self->dependency);

  super_dtor(LaudOperator, self);
  return self;
}
static const void *dependency(const void *operator) {
  return ((const struct LaudStack *)((struct LaudOperator *)(operator))
              ->dependency);
}

static int reserve_dependency_array(void *self_, int count) {
  struct LaudOperator *self = self_;
  self->dependency = LaudStackFn.Stack(count);
  return count;
}

static void *push_dependency(void *self_, void *dependency) {
  struct LaudOperator *self = self_;

  LaudStackFn.push(self->dependency, dependency);
  if (!LaudStackFn.count(self->dependency) ||
      (!LaudVarIsContinous(self) && LaudVarIsContinous(dependency))) {
    LaudVarSetContinuity(self, 1);
  }

  reference(dependency);
  return dependency;
}

static char is_operator(const void *self) {
  const void *class = classOf(self);
  while (class && class != LaudOperator && class != LaudVar &&
         class != LaudConst && (class != Ubject) && class != TypeClass) {
    class = super(class);
  }
  return class == LaudOperator;
}

static void *multiply_respects(const void *a, const void *b) {
  return LaudVarProduct(a, b);
}

static const void *
LaudOperatorComputeDerivative(const void *self_, const void *d_var_d_self,
                              struct LaudQueue *derivatives) {
  const struct LaudOperatorClass *class = classOf(self_);
  if (!class) {
    UbjectError.error("LaudOperator: object class not found\n");
  } else if (!class->computeDerivative) {
    UbjectError.error("%s: computeDerivative function missing\n",
                      className(self_));
  }
  return class->computeDerivative(self_, d_var_d_self, derivatives);
}
const struct LaudOperatorProtected LaudOperatorProtected = {
    dependency,  reserve_dependency_array, push_dependency,
    is_operator, multiply_respects,        LaudOperatorComputeDerivative};

#if 0
void breadthFirst(const struct LaudOperator *self, struct LaudQueue *q,
    struct LaudStack *stack, char unvisited) {
    const void *node = LaudQueueFn.dequeue(q);

    if (!node)
    return;

    if (isLaudOperator(node)) {
        int i = 0;
        const struct LaudOperator *op = node;
        while (i < LaudStack_count(op->dependency)) {
            const void *dependency = LaudStackFn.peek(op->dependency, i);
            if (LaudVar_getVisitValue(dependency) == unvisited) {
                LaudQueueFn.enqueue(q, dependency);
                set_visited(dependency, unvisited);
            }
            i++;
        }
    }
    Laudpush(stack, node);
    breadthFirst(self, q, stack, unvisited);
}


static void *continuityList(const struct LaudOperator *self) {
    if (!LaudVarIsContinous(self))
    return NULL;

    //  struct LaudVar **const dependencyList = self->dependencyList;

    int no_of_verts = 0; //?
    int array_length = 0; //??

    struct LaudQueue *q = LaudQueue(LaudStack_count(self->dependency));
    LaudQueueFn.enqueue(q, self);
    char unvisited = get_visit(self);
    toggle_visited(self);

    struct LaudStack *stack = LaudStack(LaudStack_count(self->dependency));
    breadthFirst(self, q, stack, unvisited);
}
#endif

// static inline char isDerivativeRespected(const void *self_) {
//   return ((struct LaudOperator *)self_)->derivativeRespected;
// }
static inline void derivativeRespect(const void *self_, void *dx) {
  if (dx) {
    ((struct LaudOperator *)self_)->derivativeRespected = 1;
    blip(((struct LaudOperator *)self_)->derivative);
    ((struct LaudOperator *)self_)->derivative = dx;
    if (getReference(dx) != 0)
      reference(dx);
  } else {
    ((struct LaudOperator *)self_)->derivativeRespected = 0;
  }
}

static inline void updateRespect(struct LaudOperator *self,
                                 const struct LaudVar *dx) {
  struct LaudVar *dx_ = self->derivative;
  if (getReference(dx_) == 0) {
    // this object belongs to me
    LaudVarProtected.setValue(dx_, LaudVarProtected.getValue(dx) +
                                       LaudVarProtected.getValue(dx_));
  } else {
    // this object is not mine
    // create a new one
    derivativeRespect(self, LaudVarAdd((const struct LaudVar *)dx_, dx));
  }
}
static void updateDerivative(struct LaudHashMap *derivatives, struct LaudVar *x,
                             void *dydx) {
  int hash;
  const void *dydx_ = LaudHashMapFn.find(derivatives, x, &hash);
  if (!dydx_)
    LaudHashMapFn.insert(derivatives, x, dydx);
  else {
    if (getReference((void *)dydx_) == 0) {
      // this object belongs to me
      LaudVarProtected.setValue((void *)dydx_,
                                LaudVarProtected.getValue(dydx_) +
                                    LaudVarProtected.getValue(dydx));
    } else {
      // this object is not mine
      // create a new one
      LaudHashMapFn.replace_key_using_hash(
          derivatives, x, LaudVarAdd((const struct LaudVar *)dydx_, dydx),
          hash);
    }
  }
}
// must be final
static void LaudVar_differentiate(const void *self_, const void *d_var_d_self,
                                  struct LaudHashMap *ddx) {
  if (LaudVarIsContinous(self_)) {
    if (is_operator(self_)) {
      const struct LaudOperator *self = self_;

      d_var_d_self = d_var_d_self ? d_var_d_self : One;
      // list of derivatives
      struct LaudQueue *derivatives = LaudQueueFn.Queue(
                           LaudStackFn.count(self->dependency)),
                       *temp = LaudQueueFn.Queue(2);
      // return list of derivatives
      LaudQueueFn.enqueue(derivatives, self);
      reference((void *)d_var_d_self);
      LaudQueueFn.enqueue(derivatives, d_var_d_self);
      // for each in self.dependency
      // differentiate
      const void *x;

      while ((x = LaudQueueFn.dequeue(derivatives))) {
        is_operator(x);
        const void *d_var_d_x = LaudQueueFn.dequeue(derivatives);
        if (is_operator(x))
          LaudOperatorComputeDerivative(x, d_var_d_x, derivatives);
        else {
          if (LaudVarIsContinous(x)) {
            printf("var: ");
            updateDerivative(ddx, (void *)x, (void *)d_var_d_x);

          } else
            printf("const: ");
        }
      }
    }
  }
}

static void *LaudOperatorClass_ctor(void *self_, va_list *args) {
  struct LaudOperatorClass *self = super_ctor(LaudOperatorClass, self_, args);
  typedef void (*voidf)();
  voidf selector;
  va_list arg = *args;
  while ((selector = va_arg(arg, voidf))) {
    voidf method = va_arg(arg, voidf);
    if (selector == (voidf)LaudOperatorComputeDerivative)
      *(voidf *)&self->computeDerivative = method;
  }
  return self;
}

// initialize library section
const void *LaudOperatorClass = NULL, *LaudOperator = NULL;
static void __attribute__((constructor(24))) initLaudOperator(void) {
  if (!LaudOperatorClass)
    LaudOperatorClass =
        init(TypeClass, LaudVarClass, sizeof(struct LaudOperatorClass), ctor,
             LaudOperatorClass_ctor, NULL);
  if (!LaudOperator)
    LaudOperator = init(LaudOperatorClass, LaudVar,
                        sizeof(struct LaudOperator), // class, parent, size
                        ctor, LaudOperator_ctor, className, "LaudOperator",
                        dtor, LaudOperator_dtor, LaudVarDifferentiate,
                        LaudVar_differentiate, NULL);
}
