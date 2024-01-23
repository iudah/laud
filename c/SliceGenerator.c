#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ubject.h>

#include "Var.h"

#include "Slice.r.h"

#define LAUD_SLICE_GENERATOR_PRIORITY (LAUD_SLICE_PRIORITY + 2)
#define SLICE_GENERATOR_C

struct LaudSliceGenerator {
#ifndef SLICE_GENERATOR_C
  char ___[sizeof(struct ___ {
#endif
    struct LaudSlice _; /**< Base LaudSlice structure. */
    size_t *origin;
    size_t *limit;
#ifndef SLICE_GENERATOR_C
  })];
#endif
};
#undef SLICE_GENERATOR_C

const void *LaudSliceGenerator;

const void *laud_generate_slices(const void *input_var,
                                 const char *slice_format, ...) {
  size_t rank_of_input_var = laud_rank(input_var);
  size_t new_shape[rank_of_input_var];
  size_t new_length = 1;

  va_list args;
  va_start(args, slice_format);

  struct LaudSliceObject *slice_object = create_slice_object(
      input_var, slice_format, &args, new_shape, &new_length);
  va_end(args);

  struct LaudSliceGenerator *this =
      init(LaudSliceGenerator, rank_of_input_var, new_shape, 0, NULL, input_var,
           slice_object);
  this->origin = malloc(rank_of_input_var * sizeof(size_t));
  this->limit = malloc(rank_of_input_var * sizeof(size_t));

  for (size_t i = 0; i < rank_of_input_var; i++) {
    this->origin[i] = slice_object[i].start;
    this->limit[i] = slice_object[i].end;
  }

  return this;
}

const void *laud_yield(const void *slice_generator) {
  const struct LaudSliceGenerator *generator = slice_generator;

  const struct LaudSliceObject *generator_data =
      get_slice_info((const struct LaudSlice *)generator);

  struct LaudSliceObject *current_slice =
      malloc(laud_rank(generator) * sizeof(struct LaudSliceObject));
  char building_slice_info = 1;
  for (size_t i = laud_rank(generator); i > 0;) {
    i--;
    current_slice[i].step = generator_data[i].step;
    current_slice[i].start =
        generator->origin[i];                   // + generator_data[i].stride;
    current_slice[i].end = generator->limit[i]; // + generator_data[i].stride;

    if (building_slice_info) {

      generator->origin[i] += generator_data[i].stride;
      generator->limit[i] += generator_data[i].stride;

      if (generator->limit[i] > laud_shape(get_slice_dependency(
                                    (const struct LaudSlice *)generator))[i]) {

        generator->origin[i] = generator_data[i].start;
        generator->limit[i] = generator_data[i].end;
        building_slice_info = 1;
      } else {
        building_slice_info = 0;
      }
    }
  }

  struct LaudSlice *sliced_var = init(
      LaudSlice, laud_rank(generator), laud_shape(generator), 0, NULL,
      get_slice_dependency((const struct LaudSlice *)generator), current_slice);

  laud_evaluate(sliced_var);

  return sliced_var;
}

static const void *laud_slice_generator_evaluate(void *slice_generator) {
  // The evaluation function for LaudSliceGenerator does nothing but return
  // itself.
  return slice_generator;
}
static const void *laud_slice_generator_differentiate(void *self) {
  struct LaudSliceGenerator *this = self;
  UbjectError.warn("did nothing on %p\n", this);
  return NULL;
}

const void *LaudSliceGenerator = NULL;

static void __attribute__((constructor(LAUD_SLICE_GENERATOR_PRIORITY)))
initLaudShuffle(void) {
  // Initialize LaudSliceGenerator if not already initialized
  if (!LaudSliceGenerator) {
    LaudSliceGenerator = init(
        LaudVarClass, LaudSlice, sizeof(struct LaudSliceGenerator), className,
        "LaudSliceGenerator", laud_evaluate, laud_slice_generator_evaluate,
        laud_differentiate, laud_slice_generator_differentiate, NULL);
  }
}
