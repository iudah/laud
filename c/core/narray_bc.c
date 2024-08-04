#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Ubject.h>

#define LAUD_NARRAY_BC_IMPLEMENTATION
#include "../core/base.r.h"
#include "../core/narray_bc.r.static.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *laud_narray_bc_ctor(void *self, va_list *args);

static void *narray_bc_dtor(void *self);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudNArrayBroadcast;

static void finish_lib() { FREE((void *)LaudNArrayBroadcast); }

static void __attribute__((constructor(LAUD_NARRAY_BC_PRIORITY)))
library_initializer(void) {

  if (!LaudNArrayBroadcast) {
    LaudNArrayBroadcast =
        init(LaudBaseClass, LaudNArray,
             sizeof(struct laud_narray_bc),    // class  parent size
             ctor, laud_narray_bc_ctor,        // constructor
             dtor, narray_bc_dtor,             //  destructor
             className, "LaudNArrayBroadcast", // class name
             NULL);
  }

  atexit(finish_lib);
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_narray_bc(const uint16_t rank, const uint64_t *const shape,
                     const uint64_t data_src_length, const number_t *const data,
                     const uint64_t *const multiplier_a,
                     const uint64_t *const multiplier_b) {
  struct laud_narray_bc *narray_bc =
      init(LaudNArrayBroadcast, rank, shape, data_src_length, data,
           multiplier_a, multiplier_b);
  if (!narray_bc) {
    // Handle memory allocation failure.
    UbjectError.error("laud_narray_bc: memory allocation failed.\n");
  }
  return narray_bc;
}

static void *laud_narray_bc_ctor(void *self, va_list *args) {
  struct laud_narray_bc *this = super_ctor(LaudNArrayBroadcast, self, args);

  this->multiplier_a = va_arg(*args, uint64_t *);
  this->multiplier_b = va_arg(*args, uint64_t *);

  return this;
}

static void *narray_bc_dtor(void *self) {
  struct laud_narray_bc *narray = self;

  if (narray->multiplier_a) {
    if (!narray->multiplier_b) {
      void **multipliers = (void **)narray->multiplier_a;
      while (*multipliers) {
        FREE(*multipliers);
        multipliers++;
      }
    } else {
      FREE(narray->multiplier_b);
    }
    FREE(narray->multiplier_a);
  }

  // UbjectError.warn("destroyed %s data @ %p\n", className(narray), narray);

  return super_dtor(LaudNArrayBroadcast, narray);
}
