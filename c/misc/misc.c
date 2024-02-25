#include <Ubject.h>

#include "../core/base.r.h"

#define MISC_STATIC

#undef MISC_STATIC

void *laud_add(void *a, void *b) {
  const struct laud_base_class *class = classOf(a);
  return class->add(a, b, NULL);
}

void *laud_matrix_dot(void *a, void *b) {
  const struct laud_base_class *class = classOf(a);

  UbjectError.warn("| - - %s - - |", className(a));

  return class->matrix_dot(a, b, NULL);
}

char *laud_to_string(const void *laud_object, char *buffer,
                     const size_t buffer_limit) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->to_string(laud_object, buffer, buffer_limit);
}

const size_t *laud_shape(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->shape(laud_object);
}

size_t laud_rank(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->rank(laud_object);
}

void laud_evaluate(void *laud_object) {

  const struct laud_base_class *class = classOf(laud_object);

  return class->evaluate(laud_object, NULL);
}
