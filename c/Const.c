#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../Ubject/Ubject.h"
#include "Const.h"
#include "Var.h"
#include "Var.r.h"

struct LaudConst {
  struct LaudVar _;
};

void *LaudConst_ctor(void *self_, va_list *arg) {
  struct LaudConst *self = super_ctor(LaudConst, self_, arg);
  LaudVarSetContinuity(self, 0);
  return self;
}

static void LaudVar_differentiate(const void *self_, const void *derivative,
                                  struct LaudHashMap *ddx_queue) {
  reference((void *)Zero);
  LaudHashMapFn.insert(ddx_queue, self_, Zero);
}

void *LaudConst_(float value) {
  struct LaudConst *self = init(LaudConst, value);
  if (!self) {
    // Handle memory allocation failure.
    UbjectError.error("Error: Memory allocation failed.\n");
    exit(1);
  }
  return self;
}

const void *LaudConst = 0;
const void *One = 0;
const void *MinusOne = 0;
const void *Zero = 0;

void __attribute__((constructor(23))) initLaudConst() {
  if (!LaudConst) {
    LaudConst =
        init(LaudVarClass, LaudVar, sizeof(struct LaudConst), // important!
             ctor, LaudConst_ctor, className, "LaudConst", LaudVarDifferentiate,
             LaudVar_differentiate, NULL);
    if (!LaudConst) {
      // Handle initialization failure.
      fprintf(stderr, "Error: LaudConst initialization failed.\n");
      exit(1);
    }

    MinusOne = init(LaudConst, -1.);
    One = init(LaudConst, 1.);
    Zero = init(LaudConst, 0.);

    if (!MinusOne || !One || !Zero) {
      // Handle initialization failure.
      fprintf(stderr, "Error: Constant initialization failed.\n");
      exit(1);
    }
  }
  return;
}