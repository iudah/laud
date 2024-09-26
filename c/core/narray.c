#include "TypeClass.r.h"
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include <Ubject.h>

#define LAUD_NARRAY_IMPLEMENTATION
#define LAUD_NARRAY_BC_IMPLEMENTATION
#define LAUD_NARRAY_BC_IMPLEMENTATION_REQUIRED
#define LAUD_SLICE_IMPL
#include "../core/base.r.h"
#include "../core/narray.h"
#include "../core/narray.r.h"
#include "../core/narray.r.static.h"
#include "../core/narray_bc.r.h"
#include "../math/common/add/add.h"
#include "../math/common/add/add.x.narray.h"
#include "../math/common/matrix_dot/matrix_dot.h"
#include "../math/common/matrix_dot/matrix_dot.xtern.h"
#include "../math/nn/conv/conv.x.narray.h"
#include "../math/nn/nn.h"
#include "../math/nn/nn.xtern.h"
#include "../math/nn/pool/max/max_pool.x.narray.h"
#include "../math/nn/pool/pool.x.narray.h"
#include "../math/others/user_elementary_fn/user_elementary_fn.h"
#include "../math/others/user_elementary_fn/user_elementary_fn.xtern.h"
#include "../misc/slice/slice.h"
#include "../misc/slice/slice.x.narray.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void laud_configure_narray(void *laud_handle, const uint16_t rank,
                                  const uint64_t *const shape,
                                  const uint64_t length,
                                  const number_t *const data);

static number_t simple_rng();

static inline char *get_line(FILE *f, uint64_t *string_length);

static void *laud_narray_ctor(void *self, va_list *args);

static void *narray_dtor(void *self);

static uint64_t narray_puto(void *self, FILE *f);

static void *narray_rollb(void *self, FILE *f);

static char *narray_as_string(const void *laud_object, char *buffer,
                              const uint64_t buf_limit);

static void *narray_relu(const struct laud_narray *operand_a);

static void *narray_sigmoid(const struct laud_narray *operand_a);

static void *narray_binary_cross_entropy(const struct laud_narray *operand_a,
                                         const struct laud_narray *operand_b);

static void *narray_reduce(const void *operand, uint16_t axis,
                           number_t (*callback)(const number_t current_net,
                                                const number_t *const values,
                                                const void *args),
                           const void *args, void *null);

static void *narray_mse(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b);

static void evaluate(const struct laud_narray *operand);

static void differentiate(const struct laud_narray *operand,
                          const struct laud_narray *pre_dydx);

static number_t value_at_offset(void *narray_, uint64_t offset);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudNArray;

static void finish_lib() { FREE((void *)LaudNArray); }

static void __attribute__((constructor(LAUD_NARRAY_PRIORITY)))
library_initializer(void) {

  if (!LaudNArray) {
    LaudNArray = init(LaudBaseClass, LaudBase,
                      sizeof(struct laud_narray),         // class  parent size
                      ctor, laud_narray_ctor,             // constructor
                      dtor, narray_dtor,                  //  destructor
                      className, "LaudNArray",            // class name
                      puto, narray_puto,                  // puto
                      rollback, narray_rollb,             // rollback
                      laud_to_string, narray_as_string,   // to string
                      laud_slice, narray_slice,           // slice
                      laud_matrix_dot, narray_matrix_dot, // matrix dot
                      laud_add, narray_add_,              // addition
                      laud_relu, narray_relu,             // relu
                      laud_sigmoid, narray_sigmoid,       // sigmoid
                      laud_conv, narray_conv,             // conv
                      laud_pool, narray_pool,             // pool
                      laud_shape, shape,                  // shape
                      laud_rank, rank,                    // rank
                      laud_evaluate, evaluate,            // evaluate
                      laud_differentiate, differentiate,  // differentiate
                      laud_reduce, narray_reduce,         // reduce
                      laud_binary_cross_entropy,          // X-entropy
                      narray_binary_cross_entropy,        // log loss
                      laud_mse, narray_mse,               // mse
                      laud_user_elementary_fn,
                      narray_user_elementary_fn, // user_elementary_fn
                      laud_value_at_offset, value_at_offset, //
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

void *laud_narray(const uint16_t rank, const uint64_t *const shape,
                  const uint64_t prefill_data_length,
                  const number_t *const prefill_number_t_array) {
  struct laud_narray *narray = init(
      LaudNArray, rank, shape, prefill_data_length, prefill_number_t_array);
  if (!narray) {
    // Handle memory allocation failure.
    UbjectError.error("laud_narray: memory allocation failed.\n");
  }
  return narray;
}

static void *laud_narray_ctor(void *self, va_list *args) {
  struct laud_narray *this = self;

  const uint32_t rank = va_arg(*args, uint32_t);

  const uint64_t *const shape = va_arg(*args, const uint64_t *const);

  const uint64_t length = va_arg(*args, uint64_t);

  const number_t *const data = va_arg(*args, const number_t *const);

  if (length != 0 && data == NULL) {
    UbjectError.error("laud_narray: data(NULL)-length(%" PRIu64 ") mismatch",
                      length);
    return this;
  }

  laud_configure_narray(this, rank, shape, length, data);

  return this;
}

static void laud_configure_narray(void *laud_handle, const uint16_t rank,
                                  const uint64_t *const shape,
                                  const uint64_t length,
                                  const number_t *const data) {

  struct laud_narray *narray = laud_handle;

  narray->rank = rank;
  narray->shape = CALLOC(rank, sizeof(uint64_t));
  if (narray->shape == NULL) {
    UbjectError.error("laud_configure_narray: memory allocation failed.\n");
  }

  narray->length = 1;

  for (uint16_t i = 0; i < rank; i++) {
    narray->length *= narray->shape[i] = shape[i];
  }

  // set up values' holder
  narray->values = CALLOC(narray->length, sizeof(number_t));
  if (narray->values == NULL) {
    UbjectError.error("laud_configure_var: memory allocation failed.\n");
  }

  // fill values with provided data
  if (data) {
    for (uint64_t i = 0; i < narray->length; i++) {
      narray->values[i] = data[i % length];
    }
  }
  return;
}

static inline void free_and_null(void **ptr) {
  if (*ptr) {
    FREE(*ptr);
    *ptr = NULL;
  }
}

static void *narray_dtor(void *self) {
  struct laud_narray *narray = self;

  free_and_null(&narray->shape);
  free_and_null(&narray->meta_data);
  free_and_null(&narray->values);
  blip(narray->computation_node);

  // UbjectError.warn("destroyed %s data @ %p\n", className(narray), narray);

  return super_dtor(LaudNArray, narray);
}

static uint64_t narray_puto(void *self, FILE *f) {
  struct laud_narray *narray = self;
  uint64_t len = fwrite(&narray->rank, 1, sizeof(narray->rank), f);
  len += fwrite(narray->shape, narray->rank, sizeof(*narray->shape), f);
  len += fwrite(&narray->length, 1, sizeof(narray->length), f);
  for (uint64_t i = 0; i < narray->length; i++) {
    // todo: check to update frexp
    int exp_ = 0;

    // using float since some systems(?) might not differentiate double type
    // from float type(?)
    int32_t sig =
        (int32_t)(frexpf((float)narray->values[i], &exp_) * (float)INT32_MAX);
    len += fwrite(&sig, 1, sizeof(sig), f);
    len += fwrite(&exp_, 1, sizeof(exp_), f);
  }
  return len;
}

static void *narray_rollb(void *self, FILE *f) {
  struct laud_narray *narray = super_rollback(LaudNArray, self, f);
  fread(&narray->rank, 1, sizeof(narray->rank), f);

  narray->shape = MALLOC(narray->rank * sizeof(*narray->shape));
  fread(narray->shape, narray->rank, sizeof(*narray->shape), f);

  fread(&narray->length, 1, sizeof(narray->length), f);

  narray->values = MALLOC(narray->length * sizeof(number_t));

  for (uint64_t i = 0; i < narray->length; i++) {
    int exp_ = 0;
    int32_t sig;
    fread(&sig, 1, sizeof(sig), f);
    fread(&exp_, 1, sizeof(exp_), f);

    narray->values[i] = (number_t)ldexpf((float)sig / (float)INT32_MAX, exp_);
  }
  return narray;
}

void *laud_from_function(
    number_t (*generator)(const uint16_t rank, const uint64_t *const shape,
                          const uint64_t offset, const void *const usr_args),
    const uint16_t rank, uint64_t *shape, const void *usr_args) {
  if (!generator) {
    typedef void(voidf)();
    voidf *simple_rng_ = (voidf *)simple_rng;
    memcpy(&generator, &simple_rng_, sizeof(simple_rng_));
  }

  struct laud_narray *narray = laud_narray(rank, shape, 0, NULL);

  uint64_t index[rank];
  memset(index, 0, sizeof(index));

  for (uint64_t i = 0; i < length(narray); i++) {
    narray->values[i] = generator(rank, index, i, usr_args);

    uint64_t j = rank - 1;
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

  return narray;
}

static _Thread_local unsigned int seed = 0;
number_t simple_rng() {
  // TODO: use pcg_rng
  if (!seed) {
    seed = (unsigned int)time(NULL);
    srand(101017);
  }

  // srand(seed);
  // return 0.0195132;
  // return 0.0495132;
  return 0.0595132;
  // return 0.0795132;
  // return 0.1;
  //         ((0.085132 - 0.0095132) * (((number_t)rand()) /
  //         (number_t)RAND_MAX));
}

void *laud_from_text(const char *file_path, const char *delim) {
  // TODO: allow internet download of text
  //  open file
  FILE *txt = fopen(file_path, "r");
  if (!txt) {
    // error occured
    UbjectError.warn("laud_from_text: failed to open file: %s",
                     strerror(errno));
    return NULL;
  }

  uint16_t data_rank = 2;
  uint64_t data_length = 0;
  uint64_t data_capacity = 1;
  uint64_t data_shape[] = {1, 1};
  struct laud_narray *narray = laud_narray(data_rank, data_shape, 0, NULL);

  number_t *data = narray->values;

  int row = 0, col = 0;
  uint64_t string_length;
  char *string;

  while ((string = get_line(txt, &string_length))) {
    char *tok = strtok(string, delim);
    while (tok) {
      if (!row)
        col++;

      if (data_length == data_capacity) {
        data_capacity *= 2;
        number_t *temp = REALLOC(data, data_capacity * sizeof(number_t));
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
    number_t *temp = REALLOC(data, data_length * sizeof(number_t));
    if (temp == NULL) {
      UbjectError.error(
          "laud_from_text: memory allocation failed after parsing");
    }
    data = temp;
  }

  uint64_t *shape = narray->shape;
  uint64_t *length = &narray->length;

  shape[0] = row;
  shape[1] = col;
  *length = shape[0] * shape[1];

  narray->values = data;

  return narray;
}

static _Thread_local char *string = NULL;
static void free_line() { FREE(string); }

static char *get_line(FILE *f, uint64_t *string_length) {
  static _Thread_local uint64_t capacity = 0;
  int c;
  uint64_t length = 0;

  if (!string_length || !f) {
    return NULL;
  }

  if (!string) {
    // Allocate initial buffer if it's the first read
    capacity = 256;
    string = CALLOC(capacity, sizeof(char));

    if (string == NULL) {
      // Handle memory allocation failure
      return 0;
    }
    atexit(free_line);
  }

  while ((c = fgetc(f)) != EOF) {
    if (length == (capacity - 1)) {
      // Expand buffer when reaching capacity
      capacity *= 2;
      char *temp = REALLOC(string, capacity * sizeof(char));
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

static inline int max(int a, int b) { return a > b ? a : b; }

static char *narray_as_string(const void *laud_object, char *buffer,
                              const uint64_t buf_limit) {
  const struct laud_narray *narray_instance = laud_object;

  uint64_t col = shape(narray_instance)[rank(narray_instance) - 1];
  const number_t *const data = values(narray_instance);
  uint64_t data_length = length(narray_instance);
  uint64_t length = data_length <= 100 ? data_length : 100;
  uint64_t back = 0, rear = 0;
  for (uint64_t i = 0; i < length; i++) {
    uint64_t rtmp, tmp = snprintf(buffer, 18, "%g", data[i]);
    rear = max(rear, rtmp = strlen(strtok(buffer, ".")));
    back = max(back, tmp - rtmp - 1);
  }

  char fmt[255];
  snprintf(fmt, 255, "%%%" PRIu64 ".%" PRIu64 "f  ", rear + back + 1, back);

  uint64_t i = 0, string_length = 0;
  while (i < length) {

    // Check if there is enough space in the buffer
    if (buf_limit - 6 <= string_length) {
      string_length = buf_limit - 6;
      break;
    }

    uint64_t remaining_space = buf_limit - string_length;

    string_length += (uint64_t)snprintf(buffer + string_length, remaining_space,
                                        fmt, data[i]);

    if ((i % col) == (col - 1)) {
      {
        string_length += (uint64_t)snprintf(buffer + string_length,
                                            buf_limit - string_length, "\n");
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

#if 0
static void *narray_slice(const void *array, const char *slice_fmt) {

  uint16_t input_rank = rank(array);
  uint64_t new_shape[input_rank];
  uint64_t new_length = 1;

  struct laud_dim_slice_data *slice_data =
      laud___create_slice_data_(array, slice_fmt, new_shape, &new_length);

  struct laud_narray *sliced_var = laud_narray(input_rank, new_shape, 0, NULL);

void *res= laud___narray_slice_array_(array, slice_data, sliced_var);
  
FREE(slice_data);
  return res;
}


static void adjust_slice_boundary(const uint16_t section,
                                  struct laud_dim_slice_data *slice,
                                  const int bound, const uint16_t dim,
                                  const uint64_t *shape) {
  uint64_t *current_field;

  switch (section) {
  case 0:
    current_field = &slice->start;
    break;
  case 1:
    current_field = &slice->stop;
    break;
  case 2:
    current_field = &slice->step;
    break;
  case 3:
    current_field = &slice->stride;
    break;
  default:
    UbjectError.error("assess_section: invalid section index: %i", section);
    return;
  }

  current_field[0] = (uint64_t)(bound >= 0 ? bound : (shape[dim] + bound));

  if ((section == 0 && slice->start >= shape[dim]) ||
      (section == 1 && slice->stop > shape[dim])) {
    UbjectError.error("assess_section: slice %s (%i) out of bound (%" PRIu64
                      ")",
                      section ? "end" : "start", bound, shape[dim]);
  }
}

static void report_expected_integer_error(const char *fmt, const char *slice) {
  if (!fmt || !slice) {
    UbjectError.error("expected_int: invalid input in expected_int");
    return;
  }

  uint64_t info_size = 2 + (uint64_t)(fmt - slice);
  char info[info_size];
  memset(info, '~', info_size - 1);
  info[info_size - 2] = '^';
  info[info_size - 1] = 0;

  UbjectError.error("expected integer before ','\n%s\n%s", slice, info);
}

static inline void fill_slice_data_for_dimension(
    const void *self, struct laud_dim_slice_data *slice_object,
    uint64_t *new_length, uint64_t *new_shape, int16_t *dim, int16_t *section,
    char *ignore_colon) {
  const uint64_t *shape_of_self = shape(self);
  while (*section < 4) {
    uint64_t value;

    if (*section == 0) {
      value = 0;
    } else if ((*section == 1) || (*section == 3)) {
      if (*ignore_colon) {
        value = slice_object[*dim].start + 1;
      } else {
        value = shape_of_self[*dim];
      }
    } else {
      value = 1;
    }

    adjust_slice_boundary(*section, slice_object + *dim, value, *dim,
                          shape_of_self);

    (*section)++;
  }

  *new_length *= new_shape[*dim] =
      (slice_object[*dim].stop - slice_object[*dim].start - 1) /
          slice_object[*dim].step +
      1;
  *section = 0;
  (*dim)++;
  *ignore_colon = 0;
}

void *laud___narray_slice_array_(const struct laud_narray *array,
                                 const struct laud_dim_slice_data *slice_data,
                                 struct laud_narray *sliced_array) {

  // Apply the slice operation
  apply_slice(slice_data, (number_t *)values(sliced_array), shape(sliced_array),
              length(sliced_array), array);

  return sliced_array;
}

static void apply_slice(const struct laud_dim_slice_data *slice_object,
                        number_t *dest_values, const uint64_t *new_shape,
                        const uint64_t new_length,
                        const struct laud_narray *const unsliced_src) {
  const uint64_t *shape_of_self = shape(unsliced_src);
  apply_effective_slice(rank(unsliced_src), 0, slice_object, 0, 0,
                        new_length / new_shape[0],
                        length(unsliced_src) / shape_of_self[0], new_shape,
                        shape_of_self, dest_values, values(unsliced_src));
}

static void apply_effective_slice(const uint16_t rank, const uint16_t dim,
                                  const struct laud_dim_slice_data *slice,
                                  const uint64_t dst_cum_offset,
                                  const uint64_t src_cum_offset,
                                  const uint64_t dst_dim_multiplier,
                                  const uint64_t src_dim_multiplier,
                                  const uint64_t *const dst_shape,
                                  const uint64_t *const src_shape,
                                  number_t *dest, const number_t *const src) {
  if (rank != dim) {

    uint64_t j = 0;
    for (uint64_t i = slice[dim].start; i < slice[dim].stop;
         i += slice[dim].step) {
      if (rank - 1 != dim) {
        apply_effective_slice(rank, dim + 1, slice,
                              dst_cum_offset + j * dst_dim_multiplier,
                              src_cum_offset + i * src_dim_multiplier,
                              dst_dim_multiplier / dst_shape[dim + 1],
                              src_dim_multiplier / src_shape[dim + 1],
                              dst_shape, src_shape, dest, src);
      } else {
        dest[dst_cum_offset + j] = src[src_cum_offset + i];
      }
      j++;
    }
  }
}
#endif
char element_compatible(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b) {
  int16_t rank_a = rank(operand_a);
  int16_t rank_b = rank(operand_b);

  while (rank_a > 0 && rank_b > 0) {
    rank_a--;
    rank_b--;

    if (shape(operand_a)[rank_a] != shape(operand_b)[rank_b] &&
        (shape(operand_a)[rank_a] == 1 || shape(operand_b)[rank_b] == 1)) {
      return 0;
    }
  }
  return 1;
}

struct laud_narray *
derivative_add(struct laud_narray *var, struct laud_narray *sum_of_derivatives,
               const struct laud_narray *derivative_wrt_var) {
  if (!derivative_wrt_var)
    return NULL;

  uint64_t length_var = length(var);
  uint64_t length_dv = length(derivative_wrt_var);

  struct laud_narray *new_sum_of_derivatives = NULL;
  if (!sum_of_derivatives) {
    if (length_var == length_dv) {
      // no broadcast
      reference((void *)derivative_wrt_var);
      new_sum_of_derivatives = (struct laud_narray *)derivative_wrt_var;
    } else {
      UbjectError.error("broadcast detected?");
      // broadcast occured
      new_sum_of_derivatives = laud_narray(rank(var), shape(var), 0, NULL);

      number_t *values_Edv = values(new_sum_of_derivatives);
      number_t *values_dv = values(derivative_wrt_var);

      uint16_t rank_var = rank(var);
      uint16_t rank_dv = rank(derivative_wrt_var);

      uint64_t index[rank_dv];
      memset(index, 0, sizeof(index));

      uint64_t multiplier[rank_var];
      multiplier[rank_var - 1] = 1;

      uint64_t *shape_var = shape(var);
      uint64_t *shape_dv = shape(derivative_wrt_var);

      for (uint64_t i = 0; i < length_dv; i++) {

        uint64_t offset = 0;
        uint16_t j = 0;
        while (j < rank_var) {
          j++;
          if (!i && j) {
            multiplier[rank_var - j] =
                shape_var[rank_var - j + 1] * multiplier[rank_var - j + 1];
          }
          offset += multiplier[rank_var - j] * index[rank_dv - j];
        }

        values_Edv[offset] += values_dv[i];

        index[rank_dv - 1]++;
        int16_t index_dim = rank_dv - 1;
        while (index_dim >= 0 && index[index_dim] == shape_dv[index_dim]) {
          index[index_dim] = 0;
          if (index_dim) {
            index[--index_dim]++;
          }
        }
      }
    }
  } else {

    number_t *derivative_values = values(sum_of_derivatives);
    if (length(sum_of_derivatives) != length(derivative_wrt_var)) {
      // We are taking the assumption that any pair of derivatives with
      // different length have to be broadcast because once a variable is
      // broadcasted, its length changes no matter how little the broadcast
      // e.g (1, 2) may broadcast to (2, 2), the derivatives retain this
      // information
      UbjectError.error("Unexpected variation in lengths. Contact developers.");
    } else {
      for (uint64_t i = 0; i < length(sum_of_derivatives); i++) {
        derivative_values[i] += values(derivative_wrt_var)[i];
      }
    }
    new_sum_of_derivatives = sum_of_derivatives;
  }
  return new_sum_of_derivatives;
}

static void *narray_reduce(__attribute__((unused)) const void *operand,
                           __attribute__((unused)) uint16_t axis,
                           __attribute__((unused)) number_t (*callback)(
                               const number_t current_net,
                               const number_t *const values, const void *args),
                           __attribute__((unused)) const void *args,
                           __attribute__((unused)) void *null) {
  UbjectError.error("sorry cannot reduce your array atm");
  return NULL;
}

char is_laud_narray(const void *laud_object) {
  const void *class = classOf(laud_object);

  const void *laud_parent = super(LaudNArray);
  while ((class)) {
    if (class == LaudNArray || class == laud_parent || class == Ubject ||
        class == TypeClass) {
      return class == LaudNArray;
    }
    class = super(class);
  }

  return 0;
}

static void evaluate(const struct laud_narray *operand) {
  if (!operand->computation_node) {
    UbjectError.error("computation node is NULL");
  }
  return laud_evaluate(operand->computation_node);
}

static void differentiate(const struct laud_narray *operand,
                          const struct laud_narray *pre_dydx) {
  if (!operand->computation_node) {
    UbjectError.error("computation node is NULL");
  }
  return laud_differentiate(operand->computation_node, pre_dydx);
}

static number_t value_at_offset(void *narray_, uint64_t offset) {
  return values((narray_))[offset];
}
