#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Ubject.h>

#define LAUD_NARRAY_IMPLEMENTATION
#define LAUD_SLICE_IMPL
#include "../core/narray.r.h"

void *laud_narray_dreduce(const struct laud_narray *operand_a,
                          uint64_t __attribute__((__unused__)) respect_index,
                          struct laud_narray *pre_dx,
                          struct laud_narray *calc_result) {
  // todo: implement derivative of reduce
#if 0
  struct laud_narray *derivative =
      laud_narray(rank(operand_a), shape(operand_a), 0, NULL);
  number_t *derivative_values = values(derivative);

  for (uint64_t i = 0; i < length(derivative); i++) {
    derivative_values[i] = (pre_dx ? values(pre_dx)[i] : 1) *
                           values(calc_result)[i] *
                           (1. - values(calc_result)[i]);
  }
  return derivative;
#endif
  return NULL;
}
