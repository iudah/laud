#include <Ubject.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../core/node.r.static.h"
#include "../../core/var.h"
#include "../../core/var.r.static.h"
#include "slice.h"
#include "slice.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *laud_slice_ctor(void *self, va_list *args);

static void *laud_slice_dtor(void *self);

static void *solve_slice(struct laud_slice *slice);

static void *differentiate_slice(struct laud_slice *slice,
                                 const uint64_t operand_index,
                                 const struct laud_narray *derivative);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudSlice = NULL;
const void *LaudSliceClass = NULL;

static void finish_lib();

static void __attribute__((constructor(LAUD_SLICE_PRIORITY)))
library_initializer(void) {
  if (!LaudSliceClass) {
    LaudSliceClass = LaudVarClass;
  }
  if (!LaudSlice) {
    LaudSlice = init(LaudSliceClass, LaudVar,
                     sizeof(struct laud_slice),           // class parent size
                     className, "LaudSlice",              // class name
                     ctor, laud_slice_ctor,               // ctor
                     dtor, laud_slice_dtor,               // dtor
                     laud_evaluate_var_node, solve_slice, // evaluate_node
                     laud_differentiate_var_node,
                     differentiate_slice, // differentiate_node
                     NULL);
  }

  atexit(finish_lib);
}

static void finish_lib() { FREE(LaudSlice); }

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_slice_generator(const void *self, const char *slice_format, ...) {

  if (is_laud_narray(self)) {
    void *tmp = self;
    self = laud_var();
    laud_set_variable_value(self, tmp, NULL);
  } else {
    reference(self);
  }

  va_list args;
  va_start(args, slice_format);

  char *resolved_slice_format =
      CALLOC(2048, 1); // resolve_slice_string(slice_fmt, &args);
  vsnprintf(resolved_slice_format, 2048, slice_format, args);
  va_end(args);

  uint64_t *new_shape = NULL;
  uint64_t new_length = 1;

  struct laud_dim_slice_data *slice_data = laud___create_slice_data(
      narray(self), resolved_slice_format, &new_shape, &new_length);

  FREE(resolved_slice_format);

  const struct laud_base_class *class = classOf(self);
  void *res = class->slice(self, slice_data, new_shape, new_length, NULL);

  blip(self);

  return res;
}

void *laud_slice(const void *self, const char *slice_format, ...) {

  va_list args;
  va_start(args, slice_format);

  char *resolved_slice_format =
      CALLOC(2048, 1); // resolve_slice_string(slice_fmt, &args);
  vsnprintf(resolved_slice_format, 2048, slice_format, args);
  va_end(args);

  FREE(resolved_slice_format);

  uint64_t *new_shape = NULL;
  uint64_t new_length = 1;

  struct laud_dim_slice_data *slice_data = laud___create_slice_data(
      narray(self), slice_format, &new_shape, &new_length);

  const struct laud_base_class *class = classOf(self);
  void *res = class->slice(self, slice_data, new_shape, new_length, NULL);

  if (is_laud_narray(self)) {
    FREE(slice_data);
    if (new_shape) {
      FREE(new_shape);
    }
  }

  return res;
}

const void *laud_yield_slice(const void *slice_generator) {
  const struct laud_slice *generator = slice_generator;

  laud_evaluate(generator);

  const struct laud_dim_slice_data *generator_data = generator->slice_data;

  struct laud_dim_slice_data *current_slice = generator->current_slice_data;

  void *src_narray = narray(incoming_nodes(generator)[0]);
  uint16_t i = laud_rank(src_narray);

  if (current_slice == generator_data) {
    current_slice = *(void **)&generator->current_slice_data =
        CALLOC(i, sizeof(struct laud_dim_slice_data));
    memcpy(current_slice, generator_data,
           i * sizeof(struct laud_dim_slice_data));
  }

  while (i) {
    --i;

    current_slice[i].stop += generator_data[i].stride;
    current_slice[i].start += generator_data[i].stride;
    if (generator_data[i].step == 0) {
      UbjectError.error("step is 0");
    }

    if (current_slice[i].stop < laud_shape(src_narray)[i]) {
      break;
    } else {
      current_slice[i].stop = generator_data[i].stop;
      current_slice[i].start = generator_data[i].start;
    }
  }

  return narray(generator);
}

static void *laud_slice_ctor(void *self, va_list *args) {
  struct laud_slice *slice_instance = super_ctor(LaudSlice, self, args);

  slice_instance->slice_data = slice_instance->current_slice_data =
      va_arg(*args, struct laud_dim_slice_data *);
  slice_instance->new_shape = va_arg(*args, const uint64_t *);
  slice_instance->new_length = va_arg(*args, const uint64_t);

  reference(incoming_nodes(self)[0]);

  return slice_instance;
}

static void *laud_slice_dtor(void *self) {

  struct laud_slice *slice = self;

  if (slice->current_slice_data != slice->slice_data) {
    FREE(slice->current_slice_data);
  }
  FREE((void *)slice->slice_data);
  FREE((void *)slice->new_shape);

  blip(incoming_nodes(self)[0]);

  return super_dtor(LaudSlice, self);
}

static void *solve_slice(struct laud_slice *slice) {
  void *array = narray((struct laud_var *)incoming_nodes(slice)[0]);
  return ((struct laud_base_class *)classOf(array))
      ->slice(array, slice->current_slice_data, slice->new_shape,
              slice->new_length, NULL);
}

static void *differentiate_slice(struct laud_slice *slice,
                                 const uint64_t operand_index,
                                 const struct laud_narray *derivative) {
  abort();
  return laud_narray_dslice(narray((struct laud_var *)incoming_nodes(slice)[0]),
                            operand_index, derivative,
                            narray((struct laud_var *)slice));
}
