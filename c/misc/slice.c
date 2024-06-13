#include <Ubject.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/base.h"
#include "../core/narray.h"
#include "../misc/slice.h"
#define NODE_PROTECTED
#define VAR_PROTECTED
#define LAUD_SLICE_IMPL
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

// static char *resolve_slice_string(const char *fmt, const va_list *arg);

#define SLICER

static void *laud_slicer_ctor(void *self, va_list *args);

static void *laud_slicer_dtor(void *self);

static void
determine_slicer_limit(const struct laud_dim_slice_data *slice_object,
                       uint16_t rank, const uint64_t *shape, uint64_t *limit);

#undef SLICER

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

const void *LaudSlicer = NULL;
const void *LaudSlicerClass = NULL;

static void finish_lib() {
  FREE(LaudSlice);
  FREE(LaudSlicerClass);
  FREE(LaudSlicer);
}

static void __attribute__((constructor(LAUD_SLICE_PRIORITY)))
library_initializer(void) {

  if (!LaudSliceClass) {
    LaudSliceClass = LaudVarClass;
  }

  if (!LaudSlice) {
    LaudSlice = init(LaudSliceClass, LaudVar,
                     sizeof(struct laud_slice),           // class parent size
                     className, "LaudSlice",              // class name
                     ctor, laud_slice_ctor,               // construtor
                     dtor, laud_slice_dtor,               // destrutor
                     laud_evaluate_var_node, solve_slice, // evaluate_node
                     NULL);
  }

  if (!LaudSlicerClass) {

    LaudSlicerClass =
        init(TypeClass, TypeClass, sizeof(struct laud_slicer_class),
             // ctor, laud_base_class_ctor,
             NULL);
  }

  if (!LaudSlicer) {
    LaudSlicer = init(LaudSliceClass, LaudVar,
                      sizeof(struct laud_slicer), // class parent size
                      className, "LaudSlicer",    // class name
                      ctor, laud_slicer_ctor,     // construtor
                      dtor, laud_slicer_dtor,     // destrutor
                      // evaluate_node, solve_slice, // evaluate_node
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

static void *laud_slice_ctor(void *self, va_list *args) {
  struct laud_slice *slice_instance = super_ctor(LaudSlice, self, args);

  slice_instance->slice_format_string = va_arg(*args, const char *);

  return slice_instance;
}

static void *laud_slice_dtor(void *self) {

  FREE((char *)((struct laud_slice *)self)->slice_format_string);

  return super_dtor(LaudSlice, self);
}

static void *solve_slice(struct laud_slice *slice) {
  typedef void *(*voidf)(const struct laud_narray *, const char *);

  const voidf slice_fn = (voidf)laud_slice;

  return slice_fn(narray((struct laud_var *)incoming_nodes(slice)[0]),
                  (const char *)slice->slice_format_string);
}

void *laud_slice(const void *laud_object, const char *slice_fmt, ...) {

  va_list args;
  va_start(args, slice_fmt);
  char *resolved_slice_format =
      CALLOC(2048, 1); // resolve_slice_string(slice_fmt, &args);
  vsnprintf(resolved_slice_format, 2048, slice_fmt, args);
  va_end(args);

  const struct laud_base_class *class = classOf(laud_object);

  return class->slice(laud_object, resolved_slice_format, NULL);
}

#define SLICER

void update_slicer(struct laud_slicer *slicer_instance,
                   const char is_new_slicer) {
  // if we change the slicer source the we might need to change the slicer data
  if (!is_new_slicer) {

    // remove old slice data if we are attempting to use a new slicer
    if (slicer_instance->slice_data) {
      FREE(slicer_instance->slice_data);
    }

    // remove old sliced n-array shape if we are attempting to use a new slicer
    if (slicer_instance->new_shape) {
      FREE(slicer_instance->new_shape);
    }

    // remove old slice generator counter if we are attempting to use a new
    // slicer
    if (slicer_instance->current_count) {
      FREE(slicer_instance->current_count);
      slicer_instance->current_count = NULL;
    }
    if (slicer_instance->current_slice_data) {
      FREE(slicer_instance->current_slice_data);
      slicer_instance->current_slice_data = NULL;
    }

    // remove old slice data limit if we are attempting to use a new slicer
    if (slicer_instance->limit) {
      FREE(slicer_instance->limit);
      slicer_instance->limit = NULL;
    }
  }

  // new lenght, new shape array, and slice data
  slicer_instance->new_length = 1;
  slicer_instance->new_shape =
      CALLOC(laud_rank(slicer_instance->src_narray), sizeof(uint64_t));

  slicer_instance->slice_data = laud___create_slice_data_(
      slicer_instance->src_narray, slicer_instance->slice_format_string,
      slicer_instance->new_shape, &slicer_instance->new_length);

  // new counters/state holders
  slicer_instance->current_slice_data =
      CALLOC(laud_rank(slicer_instance->src_narray),
             sizeof(struct laud_dim_slice_data));
  memcpy(slicer_instance->current_slice_data, slicer_instance->slice_data,
         laud_rank(slicer_instance->src_narray) *
             sizeof(struct laud_dim_slice_data));

  // new limits
  slicer_instance->limit =
      CALLOC(laud_rank(slicer_instance->src_narray), sizeof(uint64_t));
  determine_slicer_limit(
      slicer_instance->slice_data, laud_rank(slicer_instance->src_narray),
      laud_shape(slicer_instance->src_narray), slicer_instance->limit);
}

static void *laud_slicer_ctor(void *self, va_list *args) {
  struct laud_slicer *slicer_instance = self;

  slicer_instance->slice_format_string = va_arg(*args, const char *);
  slicer_instance->src_narray = narray(va_arg(*args, void *));

  if (slicer_instance->src_narray) {
    update_slicer(slicer_instance, 1);
  }

  return slicer_instance;
}

static void *laud_slicer_dtor(void *self) {

  FREE((char *)((struct laud_slice *)self)->slice_format_string);

  return super_dtor(LaudSlice, self);
}

void *laud_slice_generator(const void *laud_object, const char *slice_fmt,
                           ...) {
  va_list args;
  va_start(args, slice_fmt);
  char *resolved_slice_format =
      CALLOC(2048, 1); // resolve_slice_string(slice_fmt, &args);
  vsnprintf(resolved_slice_format, 2048, slice_fmt, args);
  va_end(args);

  UbjectError.warn("%s", resolved_slice_format);

  return init(LaudSlicer, resolved_slice_format, laud_object, NULL);
}

static void
determine_slicer_limit(const struct laud_dim_slice_data *slice_object,
                       uint16_t rank, const uint64_t *shape, uint64_t *limit) {

  while (rank) {
    rank--;

    limit[rank] = (shape[rank] - slice_object[rank].stop - 1) /
                      slice_object[rank].stride +
                  1;
  }
}

const void *laud_yield_slice(const void *slice_generator) {
  const struct laud_slicer *generator = slice_generator;

  const struct laud_dim_slice_data *generator_data = generator->slice_data;

  struct laud_dim_slice_data *current_slice = generator->current_slice_data;

  uint16_t i = laud_rank(generator->src_narray);
  while (i) {
    --i;

    current_slice[i].stop += generator_data[i].stride;
    current_slice[i].start += generator_data[i].stride;
    if (generator_data[i].step == 0) {
      UbjectError.error("step is 0");
    }

    if (current_slice[i].stop < laud_shape(generator->src_narray)[i]) {
      break;
    } else {
      current_slice[i].stop = generator_data[i].stop;
      current_slice[i].start = generator_data[i].start;
    }
  }

  struct laud_narray *sliced_var = laud_narray(laud_rank(generator->src_narray),
                                               generator->new_shape, 0, NULL);

  return laud___narray_slice_array_(generator->src_narray, current_slice,
                                    sliced_var);
}

#undef SLICER