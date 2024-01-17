#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <time.h>

#include <Ubject.h>

#define VAR_C
#include "Var.h"
#include "Var.r.h"
#undef VAR_C

#include "Const.h"
static inline size_t rank(const void *a);

static void laud_configure_var(void *laud_handle, const size_t rank,
                               const size_t *const shape, const size_t length,
                               const float *const data, char is_continous) {
  if (laud_handle == LAUD_PLACEHOLDER) {
    UbjectError.warn("laud_configure_var: Cannot configure placeholder.\n");
    return;
  }
  struct LaudVar *this = laud_handle;

  // set continuity
  this->is_continous = is_continous != 0;

  // create rank, shape, and length holder
  this->rank_shape_length = malloc((2 + rank) * sizeof(size_t));
  if (this->rank_shape_length == NULL) {
    UbjectError.error("laud_configure_var: memory allocation failed.\n");
  }

  this->rank_shape_length[0] = rank;

  // length holder
  size_t *this_length = this->rank_shape_length + 1 + rank;
  *this_length = 1;

  // shape holder
  size_t *this_shape = this->rank_shape_length + 1;

  // fill shape
  for (size_t i = 0; i < rank; i++) {
    *this_length *= this_shape[i] = shape[i];
  }

  // set up values' holder
  this->values = malloc(*this_length * sizeof(float));
  if (this->values == NULL) {
    UbjectError.error("laud_configure_var: memory allocation failed.\n");
  }

  // fill values with provided data
  if (data) {
    for (size_t i = 0; i < *this_length; i++) {
      this->values[i] = data[i % length];
    }
  }
  return;
}

static void *laud_var_ctor(void *self, va_list *args) {
  struct LaudVar *this = self;

  const size_t rank = va_arg(*args, size_t);

  const size_t *const shape = va_arg(*args, const size_t *const);

  const size_t length = va_arg(*args, size_t);

  const float *const data = va_arg(*args, const float *const);

  if (length != 0 && data == NULL) {
    UbjectError.error("laud_var: data(NULL)-length(%zu) mismatch", length);
    return NULL;
  }

  laud_configure_var(this, rank, shape, length, data, 1);

  return this;
}

static void *laud_var_evaluate(void *self) { return self; }

static void
laud_var_differentiate(const void *self,
                       const void *derivative_of_top_var_wrt_self,
                       struct LaudHashMap *map_of_derivatives_wrt_x_to_each_x) {
  const void *value_to_insert =
      derivative_of_top_var_wrt_self ? derivative_of_top_var_wrt_self : One;

  LaudHashMapFn.insert(map_of_derivatives_wrt_x_to_each_x, self,
                       value_to_insert);

  // Incrementing the reference count for the inserted value
  reference((void *)value_to_insert);
}

static char laud_var_is_float(void *self_) { return rank(self_) == 0; }

static float laud_var_set_value(void *self, size_t index, float value) {
  struct LaudVar *this = self;
  const size_t len = laud_length(this);
  float old_value = 0;

  if (index < len) {
    old_value = this->values[index];
  } else {
    UbjectError.error(
        "laud_var_set_value: index %zu out of bound in length %zu", index, len);
    return old_value;
  }

  if (!laud_is_constant(self)) {
    this->values[index] = value;
  } else {
    UbjectError.warn("laud_var_set_value: cannot modify the value of a "
                     "constant variable.\n");
  }

  return old_value;
}

static float laud_var_get_value(const void *self, size_t index) {
  const struct LaudVar *this = self;
  const size_t len = laud_length(this);
  if (index >= len) {
    UbjectError.error(
        "laud_var_get_value: index %zu out of bound in length %zu", index, len);
  }
  return this->values[index];
}

static inline size_t rank(const void *a) {
  return ((const struct LaudVar *)a)->rank_shape_length[0];
}

static inline const size_t *shape(const void *self) {
  return ((const struct LaudVar *)self)->rank_shape_length + 1;
}

const struct laud_var_protected laud_var_protected = {
    .set_value = laud_var_set_value, .get_value = laud_var_get_value};

// class
static void *laud_var_class_ctor(void *self_, va_list *args) {
  struct LaudVarClass *self = super_ctor(LaudVarClass, self_, args);
  typedef void (*voidf)();
  voidf selector;
  va_list arg = *args;
  while ((selector = va_arg(arg, voidf))) {
    voidf method = va_arg(arg, voidf);
    if (method) {
      if (selector == (voidf)laud_differentiate)
        *(voidf *)&self->differentiate = method;
      else if (selector == (voidf)laud_evaluate)
        *(voidf *)&self->evaluate = method;
      else if (selector == (voidf)laud_is_float)
        *(voidf *)&self->is_float = method;
    }
  }
  return self;
}

const void *LaudVarClass = NULL, *LaudVar = NULL;

static void __attribute__((constructor(LAUD_VAR_PRIORITY)))
init_laud_var(void) {
  if (!LaudVarClass)
    LaudVarClass = init(TypeClass, TypeClass, sizeof(struct LaudVarClass), ctor,
                        laud_var_class_ctor, NULL);
  if (!LaudVar)
    LaudVar =
        init(LaudVarClass, Ubject, sizeof(struct LaudVar), ctor, laud_var_ctor,
             className, "LaudVar", laud_evaluate, laud_var_evaluate,
             laud_differentiate, laud_var_differentiate, laud_is_float,
             laud_var_is_float, laud_set_value, laud_var_set_value, NULL);
}

void *laud_var(const size_t rank, const size_t *const shape,
               const size_t data_length, const float *data) {
  struct LaudVar *self = init(LaudVar, rank, shape, data_length, data);
  if (!self) {
    // Handle memory allocation failure.
    UbjectError.error("laud_var: memory allocation failed.\n");
  }
  return self;
}

void laud_delete(void *self) { blip(self); }

void *laud_copy(const void *self_) {
  reference((void *)self_);
  return (void *)self_;
}

float laud_set_value(void *self, const size_t index, const float value) {
  if (self == LAUD_PLACEHOLDER) {
    UbjectError.warn(
        "laud_set_value: Cannot set the value of a placeholder.\n");
    return 0;
  }
  return laud_var_set_value(self, index, value);
}

char laud_is_float(const void *self) {
  if (self == LAUD_PLACEHOLDER) {
    UbjectError.warn(
        "laud_is_float: Checking whether a placeholder is a float.\n");
    return 0; // placeholder is not a float.
  }
  const struct LaudVarClass *class = classOf(self);
  if (!class->is_float) {
    UbjectError.error(
        "laud_is_float: LaudVarClass is_float function missing.\n");
  }
  const struct LaudVar *this = self;
  return this->rank_shape_length[this->rank_shape_length[0] + 1] == 1;
}

size_t laud_rank(const void *a) { return rank(a); }

const size_t *laud_shape(const void *a) { return shape((a)); }

size_t laud_length(const void *self) { return shape(self)[rank(self)]; }

const float *laud_values(const void *self) {
  struct LaudVar *this = (void *)self;
  if (this && this->values) {
    return (const float *const)this->values;
  } else {
    UbjectError.warn("laud_values: Invalid or uninitialized LaudVar object.\n");
    return NULL;
  }
}

float laud_get_value(const void *self, const size_t index) {
  const struct LaudVar *this = self;
  const size_t len = laud_length(this);
  if (index >= laud_length(this)) {
    UbjectError.error("laud_get_value: index %zu out of bound in length %zu",
                      index, len);
    return 0;
  }
  return this->values[index];
}

void laud_set_continuity(void *self_, char continuous) {
  if (self_ == NULL) {
    UbjectError.error("laud_set_continuity: NULL pointer provided.\n");
    return;
  }

  struct LaudVar *self = (struct LaudVar *)self_;
  self->is_continous = continuous ? 1 : 0;
}

char laud_is_continuous(const void *self) {
  return (unsigned char)((struct LaudVar *)self)->is_continous;
}

void *laud_evaluate(void *self_) {
  const struct LaudVarClass *class = classOf(self_);
  if (!class) {
    UbjectError.error("laud_evaluate: object class not found\n");
  } else if (!class->evaluate) {
    UbjectError.error("laud_evaluate: evaluate function missing\n");
  }
  return class->evaluate(self_);
}

void *laud_create_derivative_map() { return LaudHashMapFn.HashMap(7); }

void laud_delete_derivative_map(void *map) { return LaudHashMapFn.del(map); }
size_t laud_start_derivative_map_iteration(void *map) {
  return LaudHashMapFn.iter_start(map);
}

void **laud_yield_derivative_map_value(void *map) {
  return LaudHashMapFn.yield(map);
}

int laud_differentiate(const void *self, void *derivative_of_top_var_wrt_self,
                       void *map_of_derivatives_wrt_x_to_each_x) {
  if (!self || !derivative_of_top_var_wrt_self ||
      !map_of_derivatives_wrt_x_to_each_x) {
    UbjectError.error(
        "laud_differentiate: invalid arguments in laud_differentiate\n");
    return -1;
  }
  const struct LaudVarClass *class = classOf(self);

  if (!class) {
    UbjectError.error("laud_differentiate: object class not found\n");
  }
  if (!class->differentiate) {
    UbjectError.error("laud_differentiate: differentiate function missing\n");
  }

  struct LaudHashMap *map = map_of_derivatives_wrt_x_to_each_x;
  class->differentiate(self, derivative_of_top_var_wrt_self, map);

  return LaudHashMapFn.count(map);
}

char *get_line(FILE *f, size_t *string_length) {
  static _Thread_local size_t capacity = 0;
  static _Thread_local char *string = NULL;
  int c;
  size_t length = 0;

  if (!string_length || !f) {
    return NULL;
  }

  if (!string) {
    // Allocate initial buffer if it's the first read
    capacity = 256;
    string = malloc(capacity * sizeof(char));

    if (string == NULL) {
      // Handle memory allocation failure
      return 0;
    }
  }

  while ((c = fgetc(f)) != EOF) {
    if (length == (capacity - 1)) {
      // Expand buffer when reaching capacity
      capacity *= 2;
      char *temp = realloc(string, capacity * sizeof(char));
      if (temp == NULL) {
        return NULL; // Memory reallocation failure
      }
    }

    if (c == (int)'\n') {
      break;
    } else {
      string[length] = (char)c;
      length++;
    }
  }
  if (c == EOF && !length) {
    return NULL;
  }
  string[length] = '\0';
  *string_length = length; // Update the string_length variable
  return string;
}

void *laud_from_text(const char *file_path, const char *delim) {
  // open file
  FILE *txt = fopen(file_path, "r");
  if (!txt) {
    // error occured
    UbjectError.warn("laud_from_text: failed to open file: %s",
                     strerror(errno));
    return NULL;
  }

  size_t data_rank = 2;
  size_t data_length = 0;
  size_t data_capacity = 1;
  size_t data_shape[] = {1, 1};
  struct LaudVar *a = laud_var(data_rank, data_shape, 0, NULL);

  float *data = a->values;

  int row = 0, col = 0;
  size_t string_length;
  char *string;

  while ((string = get_line(txt, &string_length))) {
    char *tok = strtok(string, delim);
    while (tok) {
      if (!row)
        col++;

      if (data_length == data_capacity) {
        data_capacity *= 2;
        float *temp = realloc(data, data_capacity * sizeof(float));
        if (temp == NULL) {
          fclose(txt);
          UbjectError.error(
              "laud_from_text: memory allocation failed during parsing");
        }
        data = temp;
      }

      // Check if the conversion is successful
      char *endptr;
      data[data_length] = strtof(tok, &endptr);
      if (*endptr != '\0') {
        fclose(txt);
        UbjectError.error("laud_from_text: invalid numeric value in input");
      }

      tok = strtok(NULL, delim);
      data_length++;
    }
    row++;
  }

  fclose(txt);

  if (data_length < data_capacity) {
    float *temp = realloc(data, data_length * sizeof(float));
    if (temp == NULL) {
      UbjectError.error(
          "laud_from_text: memory allocation failed after parsing");
    }
    data = temp;
  }

  size_t *shape = a->rank_shape_length + 1;
  size_t *length = shape + data_rank;

  shape[0] = row;
  shape[1] = col;
  *length = shape[0] * shape[1];

  a->values = data;

  return a;
}

static inline int max(int a, int b) { return a > b ? a : b; }

char *laud_as_string(void *a, char *buffer, size_t limit) {
  struct LaudVar *this = a;

  size_t col = laud_shape(this)[laud_rank(this) - 1];
  const float *const data = laud_values(this);
  size_t data_length = laud_length(this);
  size_t length = data_length <= 100 ? data_length : 100;
  size_t back = 0, rear = 0;
  for (size_t i = 0; i < length; i++) {
    size_t rtmp, tmp = snprintf(buffer, 18, "%g", data[i]);
    rear = max(rear, rtmp = strlen(strtok(buffer, ".")));
    back = max(back, tmp - rtmp - 1);
  }

  char fmt[255];
  snprintf(fmt, 255, "%%%i.%if  ", rear + back + 1, back);

  size_t i = 0, string_length = 0;
  while (i < length) {

    // Check if there is enough space in the buffer
    if (limit - 6 <= string_length) {
      string_length = limit - 6;
      break;
    }

    size_t remaining_space = limit - string_length;

    string_length +=
        (size_t)snprintf(buffer + string_length, remaining_space, fmt, data[i]);

    if ((i % col) == (col - 1)) {
      {
        string_length += (size_t)snprintf(buffer + string_length,
                                          limit - string_length, "\n");
      }
    }
    i++;
  }

  if (data_length > i) {
    if (buffer[string_length - 1] == '\n') {
      string_length -= 6;
    }
    snprintf(buffer + string_length, 6, "...");
  }
  return buffer;
}

struct LaudSliceObject {
  size_t start, end, step;
};
void assess_section(const size_t section, struct LaudSliceObject *slice,
                    const int value, const size_t dim, const size_t *shape) {
  size_t *current_field;

  switch (section) {
  case 0:
    current_field = &slice->start;
    break;
  case 1:
    current_field = &slice->end;
    break;
  case 2:
    current_field = &slice->step;
    break;
  default:
    UbjectError.error("assess_section: invalid section index: %zu", section);
    return;
  }

  current_field[0] = (size_t)(value >= 0 ? value : (shape[dim] + value));

  if ((section == 0 && slice->start >= shape[dim]) ||
      (section == 1 && slice->end > shape[dim])) {
    UbjectError.error("assess_section: slice %s (%i) out of bound (%i)",
                      section ? "end" : "start", value, shape[dim]);
  }
}
void expected_int(const char *fmt, const char *slice) {
  if (!fmt || !slice) {
    UbjectError.error("expected_int: invalid input in expected_int");
    return;
  }

  size_t info_size = 2 + (size_t)(fmt - slice);
  char info[info_size];
  memset(info, '~', info_size - 1);
  info[info_size - 2] = '^';
  info[info_size - 1] = 0;

  UbjectError.error("expected integer before ','\n%s\n%s", slice, info);
}

void effect_slice(const size_t rank, const size_t dim,
                  const struct LaudSliceObject *slice,
                  const size_t dst_cum_offset, const size_t src_cum_offset,
                  const size_t dst_dim_multiplier,
                  const size_t src_dim_multiplier,
                  const size_t *const dst_shape, const size_t *const src_shape,
                  float *dest, const float *const src) {
  if (rank != dim) {

    size_t j = 0;
    for (size_t i = slice[dim].start; i < slice[dim].end;
         i += slice[dim].step) {
      if (rank - 1 != dim)
        effect_slice(rank, dim + 1, slice,
                     dst_cum_offset + j * dst_dim_multiplier,
                     src_cum_offset + i * src_dim_multiplier,
                     dst_dim_multiplier / dst_shape[dim + 1],
                     src_dim_multiplier / src_shape[dim + 1], dst_shape,
                     src_shape, dest, src);
      else
        dest[dst_cum_offset + j] = src[src_cum_offset + i];

      j++;
    }
  }
}

void perform_slice(struct LaudSliceObject *slice_object, float *values,
                   size_t *new_shape, size_t new_length,
                   const struct LaudVar *const self) {

  effect_slice(rank(self), 0, slice_object, 0, 0, new_length / new_shape[0],
               laud_length(self) / shape(self)[0], new_shape, shape(self),
               values, laud_values(self));
}
void *laud_slice(const void *self, const char *slice, ...) {
  char c;
  char ignore_colon = 0;
  char *fmt = (char *)slice;
  struct LaudSliceObject *slice_object =
      malloc(sizeof(struct LaudSliceObject) * rank(self));

  if (!slice_object) {
    UbjectError.error("laud_slice: memory allocation failed in laud_slice\n");
    return NULL;
  }

  size_t dim = 0;
  size_t section = 0;

  size_t new_shape[rank(self)];
  size_t new_length = 1;

  while ((c = fmt[0])) {
    switch (c) {
    case '0' ... '9':
    case '-':
    case '+':
    case '.': {
      assess_section(section, slice_object + dim, (int)strtol(fmt, &fmt, 10),
                     dim, shape(self));
      ignore_colon = 1;
      fmt--;
      section++;
    } break;

    case ':':
      if (!ignore_colon) {
        assess_section(section, slice_object + dim,
                       section == 0 ? 0 : shape(self)[dim], dim, shape(self));
        section++;
      }
      ignore_colon = 0;
      break;

    case ',': {
      if (section == 0 || section == 2) {
        expected_int(fmt, slice);
      }

      while (section < 3) {
        assess_section(section, slice_object + dim,
                       section == 1
                           ? (ignore_colon ? slice_object[dim].start + 1
                                           : shape(self)[dim])
                           : 1,
                       dim, shape(self));
        section++;
      }
      section = 0;
      ignore_colon = 0;

#define SET_UP_NEW_SHAPE                                                       \
  new_length *= new_shape[dim] =                                               \
      (slice_object[dim].end - slice_object[dim].start) /                      \
      slice_object[dim].step;

      SET_UP_NEW_SHAPE;
      dim++;
    } break;

    default:
      break;
    }
    fmt++;
  }
  if (section == 0) {
    expected_int(fmt, slice);
  }

  while (dim < rank(self)) {

    while (section > 0 && section < 3) {
      assess_section(section, slice_object + dim,
                     section == 1 ? (ignore_colon ? slice_object[dim].start + 1
                                                  : shape(self)[dim])
                                  : 1,
                     dim, shape(self));
      section++;
    }
    SET_UP_NEW_SHAPE;
    dim++;
  }

#undef SET_UP_NEW_SHAPE

  struct LaudVar *new = laud_var(rank(self), new_shape, 0, NULL);

  if (new) {
    perform_slice(slice_object, (float *)laud_values(new), new_shape,
                  new_length, self);
  }

  free(slice_object);

  return new;
}
const void *laud_generate_slices(const void *x,
                                 const char __attribute__((unused)) * fmt,
                                 ...) {
  return x;
}
LAUDAPI const void *laud_yield(const void *generator) { return generator; }

float laud_rng() {
  static _Thread_local unsigned int seed = 0;

  if (!seed) {
    seed = (unsigned int)time(NULL);
  }

  return (rand_r(&seed) / (float)RAND_MAX);
}
void *laud_from_function(
    float (*generator)(const size_t rank, const size_t *const shape,
                       const size_t offset, const void *const usr_args),
    const size_t rank, size_t *shape, const void *usr_args) {
  if (!generator) {
    generator = laud_rng;
  }

  struct LaudVar *this = laud_var(rank, shape, 0, NULL);

  size_t index[rank];
  memset(index, 0, sizeof(index));

  for (size_t i = 0; i < laud_length(this); i++) {
    this->values[i] = generator(rank, index, i, usr_args);

    size_t j = rank - 1;
    // increment last dim in index
    index[j]++;

    // check if last dim in index == last dim shape
    // index out of bound?
    while (j >= 0 && index[j] >= shape[j]) {
      // adjust subsequent last dim
      index[j] = 0;
      if (j)
        index[--j]++;
    }
  }

  return this;
}
// Basic operators

void *laud_add(const void *a, const void *b) { return init(LaudAdd, a, b); }
void *laud_minus(const void *a, const void *b) { return init(LaudMinus, a, b); }
void *laud_product(const void *a, const void *b) {
  return init(LaudProduct, a, b);
}
void *laud_qoutient(const void *a, const void *b) {
  return init(LaudQoutient, a, b);
}
void *laud_matrix_dot(const void *a, const void *b) {
  return init(LaudMatrixDot, a, b);
}
