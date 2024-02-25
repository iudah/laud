#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <Ubject.h>

#include "base.r.h"
#define LAUD_NARRY_IMPLEMENTATION
#include "../math/nn_activations.h"
#define LAUD_SLICE_IMPL
#include "../misc/slice.r.h"
#include "narray.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void laud_configure_narray(void *laud_handle, const size_t rank,
                                  const size_t *const shape,
                                  const size_t length, const float *const data);

static float simple_rng();

static inline char *get_line(FILE *f, size_t *string_length);

static void *laud_narray_ctor(void *self, va_list *args);

static void *narray_dtor(void *self);

static char *narray_as_string(const void *laud_object, char *buffer,
                              const size_t buf_limit);

static void *narray_slice(const void *operand, const char *slice_fmt);

static void adjust_slice_boundary(const size_t section,
                                  struct laud_dim_slice_data *slice,
                                  const int bound, const size_t dim,
                                  const size_t *shape);

static void report_expected_integer_error(const char *fmt, const char *slice);

static inline void
fill_slice_data_for_dimension(const void *self,
                              struct laud_dim_slice_data *slice_object,
                              size_t *new_length, size_t *new_shape,
                              size_t *dim, size_t *section, char *ignore_colon);

static void apply_slice(const struct laud_dim_slice_data *slice_object,
                        float *dest_values, const size_t *new_shape,
                        const size_t new_length,
                        const struct laud_narray *const unsliced_src);

static void apply_effective_slice(
    const size_t rank, const size_t dim,
    const struct laud_dim_slice_data *slice, const size_t dst_cum_offset,
    const size_t src_cum_offset, const size_t dst_dim_multiplier,
    const size_t src_dim_multiplier, const size_t *const dst_shape,
    const size_t *const src_shape, float *dest, const float *const src);

static void *narray_matrix_dot(const struct laud_narray *operand_a,
                               const struct laud_narray *operand_b);

static inline char
operands_have_common(const size_t *operand_a_shape, const size_t a_index,
                     const size_t *operand_b_shape, const size_t b_index,
                     size_t *result_shape, size_t *common, size_t *i_multiplier,
                     size_t *j_multiplier, size_t *k_a_multiplier,
                     size_t *k_b_multiplier);

static void *narray_add(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b);

static void *narray_relu(const struct laud_narray *operand_a);

static void *narray_sigmoid(const struct laud_narray *operand_a);

static void evaluate(const struct laud_narray *operand);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudNArray;

static void __attribute__((constructor(LAUD_NARRAY_PRIORITY)))
library_initializer(void) {

  if (!LaudNArray) {
    LaudNArray = init(LaudBaseClass, LaudBase,
                      sizeof(struct laud_narray),         // class  parent size
                      ctor, laud_narray_ctor,             // constructor
                      dtor, narray_dtor,                  //  destructor
                      className, "LaudNArray",            // class name
                      laud_to_string, narray_as_string,   // to string
                      laud_slice, narray_slice,           // slice
                      laud_matrix_dot, narray_matrix_dot, // matrix dot
                      laud_add, narray_add,               // addition
                      laud_relu, narray_relu,             // relu
                      laud_sigmoid, narray_sigmoid,       // sigmoid
                      laud_shape, shape,                  // shape
                      laud_rank, rank,                    // rank
                      laud_evaluate, evaluate,            // evaluate
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

void *laud_narray(const size_t rank, const size_t *const shape,
                  const size_t data_src_length, const float *const data) {
  struct laud_narray *narray =
      init(LaudNArray, rank, shape, data_src_length, data);
  if (!narray) {
    // Handle memory allocation failure.
    UbjectError.error("laud_narray: memory allocation failed.\n");
  }
  return narray;
}

static void *laud_narray_ctor(void *self, va_list *args) {
  struct laud_narray *this = self;

  const size_t rank = va_arg(*args, size_t);

  const size_t *const shape = va_arg(*args, const size_t *const);

  const size_t length = va_arg(*args, size_t);

  const float *const data = va_arg(*args, const float *const);

  if (length != 0 && data == NULL) {
    UbjectError.error("laud_narray: data(NULL)-length(%zu) mismatch", length);
    return this;
  }

  laud_configure_narray(this, rank, shape, length, data);

  return this;
}

static void laud_configure_narray(void *laud_handle, const size_t rank,
                                  const size_t *const shape,
                                  const size_t length,
                                  const float *const data) {

  struct laud_narray *narray = laud_handle;

  narray->rank = rank;
  narray->shape = malloc(rank * sizeof(size_t));
  if (narray->shape == NULL) {
    UbjectError.error("laud_configure_narray: memory allocation failed.\n");
  }

  narray->length = 1;

  for (size_t i = 0; i < rank; i++) {
    narray->length *= narray->shape[i] = shape[i];
  }

  // set up values' holder
  narray->values = malloc(narray->length * sizeof(float));
  if (narray->values == NULL) {
    UbjectError.error("laud_configure_var: memory allocation failed.\n");
  }

  // fill values with provided data
  if (data) {
    for (size_t i = 0; i < narray->length; i++) {
      narray->values[i] = data[i % length];
    }
  }
  return;
}

static void *narray_dtor(void *self) {
  struct laud_narray *narray = self;

  free(narray->values);
  free(narray->shape);
  blip(narray->computation_node);

  UbjectError.warn("destroyed %s data @ %p\n", className(narray), narray);

  return super_dtor(LaudNArray, narray);
}

void *laud_from_function(
    float (*generator)(const size_t rank, const size_t *const shape,
                       const size_t offset, const void *const usr_args),
    const size_t rank, size_t *shape, const void *usr_args) {
  if (!generator) {
    generator = simple_rng;
  }

  struct laud_narray *narray = laud_narray(rank, shape, 0, NULL);

  size_t index[rank];
  memset(index, 0, sizeof(index));

  for (size_t i = 0; i < length(narray); i++) {
    narray->values[i] = generator(rank, index, i, usr_args);

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

  return narray;
}

static float simple_rng() {
  static _Thread_local unsigned int seed = 0;

  if (!seed) {
    seed = (unsigned int)time(NULL);
  }

  return (rand_r(&seed) / (float)RAND_MAX);
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
  struct laud_narray *narray = laud_narray(data_rank, data_shape, 0, NULL);

  float *data = narray->values;

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

  size_t *shape = narray->shape;
  size_t *length = &narray->length;

  shape[0] = row;
  shape[1] = col;
  *length = shape[0] * shape[1];

  narray->values = data;

  return narray;
}

static char *get_line(FILE *f, size_t *string_length) {
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

static inline int max(int a, int b) { return a > b ? a : b; }

static char *narray_as_string(const void *laud_object, char *buffer,
                              const size_t buf_limit) {
  const struct laud_narray *narray_instance = laud_object;

  size_t col = shape(narray_instance)[rank(narray_instance) - 1];
  const float *const data = values(narray_instance);
  size_t data_length = length(narray_instance);
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
    if (buf_limit - 6 <= string_length) {
      string_length = buf_limit - 6;
      break;
    }

    size_t remaining_space = buf_limit - string_length;

    string_length +=
        (size_t)snprintf(buffer + string_length, remaining_space, fmt, data[i]);

    if ((i % col) == (col - 1)) {
      {
        string_length += (size_t)snprintf(buffer + string_length,
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

static void *narray_slice(const void *array, const char *slice_fmt) {

  size_t input_rank = rank(array);
  size_t new_shape[input_rank];
  size_t new_length = 1;

  struct laud_dim_slice_data *slice_data =
      laud___create_slice_data_(array, slice_fmt, new_shape, &new_length);

  struct laud_narray *sliced_var = laud_narray(input_rank, new_shape, 0, NULL);

  return laud___narray_slice_array_(array, slice_data, sliced_var);
}

void *laud___create_slice_data_(const void *array,
                                const char *const slice_format,
                                size_t *new_shape, size_t *new_length) {

  char *format_cursor = (char *)slice_format;

  const size_t array_rank = rank(array);
  const size_t *array_shape = shape(array);

  struct laud_dim_slice_data *slice_object =
      malloc(sizeof(struct laud_dim_slice_data) * array_rank);

  if (!slice_object) {
    UbjectError.error(
        "create_slice_object: memory allocation failed in laud_slice\n");
    return NULL;
  }

  char ignore_colon = 0;

  size_t current_dimension = 0;
  size_t current_section = 0;

  char current_char;

  while ((current_char = format_cursor[0])) {
    switch (current_char) {
    case '0' ... '9':
    case '-':
    case '+': {
      int value = (int)strtol(format_cursor, &format_cursor, 10);

      adjust_slice_boundary(current_section, slice_object + current_dimension,
                            value, current_dimension, array_shape);
      ignore_colon = 1;
      format_cursor--;
      current_section++;
    } break;

    case ':':
      if (!ignore_colon) {
        adjust_slice_boundary(
            current_section, slice_object + current_dimension,
            current_section == 0 ? 0 : array_shape[current_dimension],
            current_dimension, array_shape);
        current_section++;
      }
      ignore_colon = 0;
      break;

    case ',': {
      if (current_section == 0 || current_section == 2) {
        report_expected_integer_error(format_cursor, slice_format);
      }
      fill_slice_data_for_dimension(array, slice_object, new_length, new_shape,
                                    &current_dimension, &current_section,
                                    &ignore_colon);

    } break;

    default:
      break;
    }
    format_cursor++;
  }
  if (current_section == 0) {
    report_expected_integer_error(format_cursor, slice_format);
  }
  // current_dimension will still be less than array_rank. This while loop will
  // remedy that and complete the shape
  while (current_dimension < rank(array)) {
    fill_slice_data_for_dimension(array, slice_object, new_length, new_shape,
                                  &current_dimension, &current_section,
                                  &ignore_colon);
  }

  return slice_object;
}

static void adjust_slice_boundary(const size_t section,
                                  struct laud_dim_slice_data *slice,
                                  const int bound, const size_t dim,
                                  const size_t *shape) {
  size_t *current_field;

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
    UbjectError.error("assess_section: invalid section index: %zu", section);
    return;
  }

  current_field[0] = (size_t)(bound >= 0 ? bound : (shape[dim] + bound));

  if ((section == 0 && slice->start >= shape[dim]) ||
      (section == 1 && slice->stop > shape[dim])) {
    UbjectError.error("assess_section: slice %s (%i) out of bound (%i)",
                      section ? "end" : "start", bound, shape[dim]);
  }
}

static void report_expected_integer_error(const char *fmt, const char *slice) {
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

static inline void fill_slice_data_for_dimension(
    const void *self, struct laud_dim_slice_data *slice_object,
    size_t *new_length, size_t *new_shape, size_t *dim, size_t *section,
    char *ignore_colon) {
  const size_t *shape_of_self = shape(self);
  while (*section < 4) {
    size_t value;

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
  apply_slice(slice_data, (float *)values(sliced_array), shape(sliced_array),
              length(sliced_array), array);

  return sliced_array;
}

static void apply_slice(const struct laud_dim_slice_data *slice_object,
                        float *dest_values, const size_t *new_shape,
                        const size_t new_length,
                        const struct laud_narray *const unsliced_src) {
  const size_t *shape_of_self = shape(unsliced_src);
  apply_effective_slice(rank(unsliced_src), 0, slice_object, 0, 0,
                        new_length / new_shape[0],
                        length(unsliced_src) / shape_of_self[0], new_shape,
                        shape_of_self, dest_values, values(unsliced_src));
}

static void apply_effective_slice(
    const size_t rank, const size_t dim,
    const struct laud_dim_slice_data *slice, const size_t dst_cum_offset,
    const size_t src_cum_offset, const size_t dst_dim_multiplier,
    const size_t src_dim_multiplier, const size_t *const dst_shape,
    const size_t *const src_shape, float *dest, const float *const src) {
  if (rank != dim) {

    size_t j = 0;
    for (size_t i = slice[dim].start; i < slice[dim].stop;
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

static void *narray_matrix_dot(const struct laud_narray *operand_a,
                               const struct laud_narray *operand_b) {
  if (rank(operand_a) != 2) {
    UbjectError.error("rank of operand 1 != 2");
  }
  if (rank(operand_b) != 2) {
    UbjectError.error("rank of operand 2 != 2");
  }

  size_t result_shape[2];
  size_t common;
  size_t i_multiplier;
  size_t j_multiplier;
  size_t k_a_multiplier;
  size_t k_b_multiplier;

  if (!operands_have_common(shape(operand_a), 1, shape(operand_b), 0,
                            result_shape, &common, &i_multiplier, &j_multiplier,
                            &k_a_multiplier, &k_b_multiplier))
    if (!operands_have_common(shape(operand_a), 0, shape(operand_b), 0,
                              result_shape, &common, &i_multiplier,
                              &j_multiplier, &k_a_multiplier, &k_b_multiplier))
      if (!operands_have_common(
              shape(operand_a), 0, shape(operand_b), 1, result_shape, &common,
              &i_multiplier, &j_multiplier, &k_a_multiplier, &k_b_multiplier))
        if (!operands_have_common(
                shape(operand_a), 1, shape(operand_b), 1, result_shape, &common,
                &i_multiplier, &j_multiplier, &k_a_multiplier, &k_b_multiplier))
          UbjectError.error("no matching dims found");

  struct laud_narray *result = laud_narray(2, result_shape, 0, NULL);
  float *result_values = values(result);

  for (size_t i = 0; i < result_shape[0]; i++) {

    for (size_t j = 0; j < result_shape[1]; j++) {

      for (size_t k = 0; k < common; k++) {
        result_values[i * result_shape[1] + j] +=
            (values(operand_a)[i * i_multiplier + k * k_a_multiplier] *
             values(operand_b)[j * j_multiplier + k * k_b_multiplier]);
      }
    }
  }
  return result;
}

static inline char
operands_have_common(const size_t *operand_a_shape, const size_t a_index,
                     const size_t *operand_b_shape, const size_t b_index,
                     size_t *result_shape, size_t *common, size_t *i_multiplier,
                     size_t *j_multiplier, size_t *k_a_multiplier,
                     size_t *k_b_multiplier) {
  if (operand_a_shape[a_index] == operand_b_shape[b_index]) {
    result_shape[0] = operand_a_shape[(a_index + 1) % 2];
    result_shape[1] = operand_b_shape[(b_index + 1) % 2];
    *common = operand_a_shape[a_index];

    if (a_index == 1) {
      *i_multiplier = operand_a_shape[1];
      *k_a_multiplier = 1;
    } else {
      *k_a_multiplier = operand_a_shape[1];
      *i_multiplier = 1;
    }

    if (b_index == 0) {
      *k_b_multiplier = operand_b_shape[1];
      *j_multiplier = 1;
    } else {
      *j_multiplier = operand_b_shape[1];
      *k_b_multiplier = 1;
    }

    return 1;
  }
  return 0;
}

static void *narray_add(const struct laud_narray *operand_a,
                        const struct laud_narray *operand_b) {
  if (length(operand_a) != length(operand_b)) {
    UbjectError.error("length of operand 1 != length of operand 2");
  }

  struct laud_narray *result =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  float *result_values = values(result);

  for (size_t i = 0; i < shape(operand_a)[0]; i++) {

    result_values[i] = (values(operand_a)[i] + values(operand_b)[i]);
  }
  return result;
}

static void *narray_relu(const struct laud_narray *operand_a) {

  struct laud_narray *result =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  float *result_values = values(result);

  for (size_t i = 0; i < shape(operand_a)[0]; i++) {

    result_values[i] = values(operand_a)[i];
    if (result_values[i] < 0)
      result_values[i] = 0;
  }
  return result;
}

static void *narray_sigmoid(const struct laud_narray *operand_a) {

  struct laud_narray *result =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  float *result_values = values(result);

  for (size_t i = 0; i < shape(operand_a)[0]; i++) {

    result_values[i] = 1 / (1 + exp(-values(operand_a)[i]));
  }
  return result;
}

char is_laud_narray(const void *laud_object) {
  const void *class;

  const void *laud_parent = super(LaudNArray);
  while ((class = classOf(laud_object))) {
    if (class == LaudNArray || class == laud_parent || class == Ubject ||
        class == TypeClass) {
      return class == LaudNArray;
    }
  }

  return 0;
}

static void evaluate(const struct laud_narray *operand) {
  if (!operand->computation_node) {
    UbjectError.error("computation node is NULL");
  }
  return laud_evaluate(operand->computation_node);
}
