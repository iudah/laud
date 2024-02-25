#include <Ubject.h>
#include <Ubject.r.h>

#include "../core/base.r.h"

void *laud_relu(void *operand) {

  const struct laud_base_class *class = classOf(operand);

  return class->relu(operand, NULL);
}

void *laud_sigmoid(void *operand) {

  const struct laud_base_class *class = classOf(operand);

  return class->sigmoid(operand, NULL);
}
