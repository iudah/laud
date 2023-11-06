#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define VAR_C
#include "Const.h"
#include "Var.h"
#include "Var.r.h"

static void *LaudVar_ctor(void *self_, va_list *args) {
  struct LaudVar *self = self_;
  self->value = (float)va_arg(*args, double);
  return self;
}

static void *LaudVar_evaluate(void *self_) {
  // TODO: implementation is not complete
  return self_;
}
static void LaudVar_differentiate(const void *self_, const void *derivative,
                                  struct LaudHashMap *ddx_queue) {
  LaudHashMapFn.insert(ddx_queue, self_, derivative ? derivative : One);
  reference((void *)(derivative ? derivative : One));
}
static char LaudVar_isFloat(void *self_) {
  // TODO: Update implementation
  return 1;
}
static float LaudVar_setValue(void *self_, float value) {
  if (LaudVarIsContinous(self_)) {
    struct LaudVar *self = self_;
    return self->value = value;
  }
  UbjectError.warn("LaudConst: ignored attempt to change value\n");
  return value;
}
static float LaudVar_getValue(const void *self_) {
  return ((struct LaudVar *)self_)->value;
}

const struct LaudVarProtected LaudVarProtected = {LaudVar_setValue,
                                                  LaudVar_getValue};

// class
static void *LaudVarClass_ctor(void *self_, va_list *args) {
  struct LaudVarClass *self = super_ctor(LaudVarClass, self_, args);
  typedef void (*voidf)();
  voidf selector;
  va_list arg = *args;
  while ((selector = va_arg(arg, voidf))) {
    voidf method = va_arg(arg, voidf);
    if (selector == (voidf)LaudVarDifferentiate)
      *(voidf *)&self->differentiate = method;
    else if (selector == (voidf)LaudVarEvaluate)
      *(voidf *)&self->evaluate = method;
    else if (selector == (voidf)LaudVarIsFloat)
      *(voidf *)&self->isFloat = method;
  }
  return self;
}

const void *LaudVarClass = NULL, *LaudVar = NULL;

static void __attribute__((constructor(22))) initLaudVar(void) {
  if (!LaudVarClass)
    LaudVarClass = init(TypeClass, TypeClass, sizeof(struct LaudVarClass), ctor,
                        LaudVarClass_ctor, NULL);
  if (!LaudVar)
    LaudVar =
        init(LaudVarClass, Ubject, sizeof(struct LaudVar), ctor, LaudVar_ctor,
             className, "LaudVar", LaudVarEvaluate, LaudVar_evaluate,
             LaudVarDifferentiate, LaudVar_differentiate, LaudVarIsFloat,
             LaudVar_isFloat, LaudVarSetFloatValue, LaudVar_setValue, NULL);
}

void *LaudVarVar(float f) { return init(LaudVar, f, NULL); }

void LaudVarDelete(void *self) { blip(self); }

void *LaudVarCopy(const void *self_) {
  reference((void *)self_);
  return (void *)self_;
}

float LaudVarSetFloatValue(void *self, const float value) {

  return LaudVar_setValue(self, value);
}

char LaudVarIsFloat(const void *self_) {
  const struct LaudVarClass *class = classOf(self_);
  if (!class->isFloat) {
    UbjectError.error("LaudVar: isFloat function missing\n");
  }
  return class->isFloat(self_);
}

float LaudVarGetFloatValue(const void *lconst) {
  const struct LaudVar *self = lconst;
  return self->value;
}

void LaudVarSetContinuity(void *self_, char continous) {
  struct LaudVar *self = self_;
  self->discontinuity_marker = continous ? 0 : 1;
}
char LaudVarIsContinous(const void *self) {
  return (unsigned char)((struct LaudVar *)self)->discontinuity_marker == 0;
}

void *LaudVarEvaluate(void *self_) {
  const struct LaudVarClass *class = classOf(self_);
  if (!class) {
    UbjectError.error("LaudVar: object class not found\n");
  } else if (!class->evaluate) {
    UbjectError.error("LaudVar: evaluate function missing\n");
  }
  return class->evaluate(self_);
}

void *LaudVarDerivativeMap() { return LaudHashMapFn.HashMap(7); }
void LaudVarDeleteDerivativeMap(void *map) { return LaudHashMapFn.del(map); }
void LaudVarDerivativeMapIterStart(void *map) {
  return LaudHashMapFn.iter_begin(map);
}
void **LaudVarDerivativeMapNext(void *map) {
  return LaudHashMapFn.iter_next(map);
}
int LaudVarDifferentiate(const void *self_, void *derivative, void *ddx) {
  const struct LaudVarClass *class = classOf(self_);
  if (!class) {
    UbjectError.error("LaudVar: object class not found\n");
  } else if (!class->differentiate) {
    UbjectError.error("LaudVar: differentiate function missing\n");
  }
  struct LaudHashMap *map = ddx; // = LaudHashMapFn.HashMap(3);
  class->differentiate(self_, derivative, map);
  return LaudHashMapFn.count(map);
}

// Basic operators

void *LaudVarAdd(const void *a, const void *b) { return init(LaudAdd, a, b); }
void *LaudVarMinus(const void *a, const void *b) {
  return init(LaudMinus, a, b);
}
void *LaudVarProduct(const void *a, const void *b) {
  return init(LaudProduct, a, b);
}
void *LaudVarQoutient(const void *a, const void *b) {
  return init(LaudQoutient, a, b);
}