#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ubject.h>

#include "Const.h"
#include "Var.h"
#include "Var.r.h"

#define LAUD_CONST_PRIORITY (LAUD_VAR_PRIORITY + 2)

struct LaudConst {
  struct LaudVar _;
};

static void *laud_const_ctor(void *self_, va_list *arg) {
  struct LaudConst *self = super_ctor(LaudConst, self_, arg);
  laud_set_continuity(self, 0);
  return self;
}

char laud_is_constant(const void *self) { return classOf(self) == LaudConst; }

static void laud_var_differentiate(const void *variable,
                                   const void *__attribute__((unused))
                                   derivative,
                                   struct LaudHashMap *derivative_queue) {
  // Increment reference count for Zero object
  reference((void *)Zero);

  // Insert the pair (self_, Zero) into ddx_queue
  LaudHashMapFn.insert(derivative_queue, variable, Zero);
}

void *laud_const(const size_t rank, const size_t *const shape,
                 const size_t data_length, const float *data) {
  // Initialize LaudConst object with provided parameters
  struct LaudConst *const_variable =
      init(LaudConst, rank, shape, data_length, data);

  // Check if memory allocation was successful
  if (!const_variable) {
    // Handle memory allocation failure.
    UbjectError.error("Error: Memory allocation failed for LaudConst.\n");
  }

  return const_variable;
}

const void *LaudConst = 0;
const void *One = 0;
const void *MinusOne = 0;
const void *Zero = 0;

void __attribute__((constructor(LAUD_CONST_PRIORITY))) initLaudConst() {
  // Check if LaudConst is not already initialized
  if (!LaudConst) {
    // Initialize LaudConst class
    LaudConst =
        init(LaudVarClass, LaudVar, sizeof(struct LaudConst), // important!
             ctor, laud_const_ctor, className, "LaudConst", laud_differentiate,
             laud_var_differentiate, NULL);

    // Check if LaudConst initialization was successful
    if (!LaudConst) {
      // Handle initialization failure.
      fprintf(stderr, "Error: LaudConst initialization failed.\n");
      abort();
    }

    // Initialize constant values: MinusOne, One, and Zero
    float minus_one = -1.0;
    MinusOne = laud_const(0, NULL, 1, &minus_one);

    float one = 1.0;
    One = laud_const(0, NULL, 1, &one);

    float zero = 0.0;
    Zero = laud_const(0, NULL, 1, &zero);

    // Check if constant initialization was successful
    if (!MinusOne || !One || !Zero) {
      // Handle initialization failure.
      fprintf(stderr, "Error: Constant initialization failed.\n");
      abort();
    }
  }
  return;
}