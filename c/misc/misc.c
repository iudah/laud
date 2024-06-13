#include <Ubject.h>
#include <stdint.h>

#include "../core/base.h"
#include "../core/base.r.h"

#define MISC_STATIC

#undef MISC_STATIC

char *laud_to_string(const void *laud_object, char *buffer,
                     const uint64_t buffer_limit) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->to_string(laud_object, buffer, buffer_limit);
}

const uint64_t *laud_shape(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->shape(laud_object);
}

uint16_t laud_rank(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->rank(laud_object);
}

void laud_evaluate(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->evaluate(laud_object, NULL);
}

void laud_differentiate(void *laud_object, const void *pre_derivatives) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->differentiate(laud_object, pre_derivatives, NULL);
}

void *laud_reduce(void *operand, uint16_t axis,
                  number_t (*callback)(const number_t current_net,
                                       const number_t *const values,
                                       const void *args),
                  const void *args) {
  const struct laud_base_class *class = classOf(operand);
  return class->reduce(operand, axis, callback, args, NULL);
}

number_t laud_value_at_offset(void *operand, uint64_t offset) {

  const struct laud_base_class *class = classOf(operand);
  return class->value_at_offset(operand, offset, NULL);
}
