#include <Ubject.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/base.h"
#include "../core/narray.h"
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

static char *resolve_slice_string(const char *fmt, const va_list *arg);

#define SLICER

static void *laud_slicer_ctor(void *self, va_list *args);

static void *laud_slicer_dtor(void *self);

static void
determine_slicer_limit(const struct laud_dim_slice_data *slice_object,
                       size_t rank, const size_t *shape, size_t *limit);

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

static void __attribute__((constructor(LAUD_SLICE_PRIORITY)))
library_initializer(void) {

  if (!LaudSliceClass) {
    LaudSliceClass = LaudVarClass;
  }

  if (!LaudSlice) {
    LaudSlice = init(LaudSliceClass, LaudVar,
                     sizeof(struct laud_slice),  // class parent size
                     className, "LaudSlice",     // class name
                     ctor, laud_slice_ctor,      // construtor
                     dtor, laud_slice_dtor,      // destrutor
                     evaluate_node, solve_slice, // evaluate_node
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

  free((char *)((struct laud_slice *)self)->slice_format_string);

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
  char *resolved_slice_format = resolve_slice_string(slice_fmt, &args);
  va_end(args);

  const struct laud_base_class *class = classOf(laud_object);

  return class->slice(laud_object, resolved_slice_format, NULL);
}

static char *resolve_slice_string(const char *slice_fmt, const va_list *arg) {
  size_t available_space = strlen(slice_fmt) + 1;
  char *string = malloc(available_space);

  size_t string_length = 0;

  char *start_point = (char *)slice_fmt;
  char *end_point;
  while ((end_point = strchr(start_point, '%'))) {

    const size_t required_space = end_point - start_point;
    strncpy(string + string_length, start_point, required_space);
    available_space -= required_space;
    string_length += required_space;

    if (available_space < 13) {
      available_space += string_length + 13;
      char *tmp = realloc(string, available_space + string_length);
      if (!tmp) {
        free(string);
        UbjectError.error("could not allocated memory to resolve slice format");
      }
      string = tmp;
    }

    string[string_length] = 0;

    end_point++;

    char *const fmts[] = {"%i", "%u", "%zu"};
    char *fmt;
    switch (end_point[0]) {
    case 'i':
      fmt = fmts[0];
      break;
    case 'u':
      fmt = fmts[1];
      break;
    case 'z':
      switch (end_point[1]) {
      case 'u':
      case 'i':
        end_point++;
        fmt = fmts[2];
        break;
      default:
        UbjectError.error("slice format expected %%zu or %%zi but got %%z%c",
                          end_point[1]);
      }
      break;
    default:
      UbjectError.error(
          "slice format expected %%u, %%i, %%zu, or %%zi but got %c",
          end_point[0]);
    }

    end_point++;

    size_t print_length =
        vsnprintf(string + string_length, available_space, fmt, *arg);
    available_space -= print_length;
    string_length += print_length;
    start_point = end_point;
  }

  strncpy(string + string_length, start_point, strlen(start_point) + 1);

  return string;
}

#define SLICER

void update_slicer(struct laud_slicer *slicer_instance,
                   const char is_new_slicer) {
  // if we change the slicer source the we might need to change the slicer data
  if (!is_new_slicer) {

    // remove old slice data if we are attempting to use a new slicer
    if (slicer_instance->slice_data) {
      free(slicer_instance->slice_data);
    }

    // remove old sliced n-array shape if we are attempting to use a new slicer
    if (slicer_instance->new_shape) {
      free(slicer_instance->new_shape);
    }

    // remove old slice generator counter if we are attempting to use a new
    // slicer
    if (slicer_instance->current_count) {
      free(slicer_instance->current_count);
      slicer_instance->current_count = NULL;
    }
    if (slicer_instance->current_slice_data) {
      free(slicer_instance->current_slice_data);
      slicer_instance->current_slice_data = NULL;
    }

    // remove old slice data limit if we are attempting to use a new slicer
    if (slicer_instance->limit) {
      free(slicer_instance->limit);
      slicer_instance->limit = NULL;
    }
  }

  // new lenght, new shape array, and slice data
  slicer_instance->new_length = 1;
  slicer_instance->new_shape =
      malloc(laud_rank(slicer_instance->src_narray) * sizeof(size_t));

  slicer_instance->slice_data = laud___create_slice_data_(
      slicer_instance->src_narray, slicer_instance->slice_format_string,
      slicer_instance->new_shape, &slicer_instance->new_length);

  // new counters/state holders
  slicer_instance->current_slice_data =
      malloc(laud_rank(slicer_instance->src_narray) *
             sizeof(struct laud_dim_slice_data));
  memcpy(slicer_instance->current_slice_data, slicer_instance->slice_data,
         laud_rank(slicer_instance->src_narray) *
             sizeof(struct laud_dim_slice_data));

  // new limits
  slicer_instance->limit =
      malloc(laud_rank(slicer_instance->src_narray) * sizeof(size_t));
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

  free((char *)((struct laud_slice *)self)->slice_format_string);

  return super_dtor(LaudSlice, self);
}

void *laud_slice_generator(const void *laud_object, const char *slice_fmt,
                           ...) {
  va_list args;
  va_start(args, slice_fmt);
  char *resolved_slice_format = resolve_slice_string(slice_fmt, &args);

  vprintf(slice_fmt, args);

  va_end(args);

  return init(LaudSlicer, resolved_slice_format, laud_object, NULL);
}

static void
determine_slicer_limit(const struct laud_dim_slice_data *slice_object,
                       size_t rank, const size_t *shape, size_t *limit) {

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

  size_t i = laud_rank(generator->src_narray);
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
