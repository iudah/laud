#ifndef BASE_R_H
#define BASE_R_H

#include <TypeClass.r.h>
#include <Ubject.r.h>

struct laud_base;

struct laud_base_class {

  const struct TypeClass _;

  void *(*add)(void *operand_a, void *operand_b, void *null);

  void *(*matrix_dot)(void *operand_a, void *operand_b, void *null);

  void *(*to_string)(const void *operand, char *buffer,
                     const size_t buffer_limit);

  void *(*slice)(const void *operand, const char *slice_format, void *null);

  void *(*relu)(const void *operand, void *null);

  void *(*sigmoid)(const void *operand, void *null);

  size_t *(*shape)(const void *operand);

  size_t (*rank)(const void *operand);

  void *(*slice_generator)(const void *operand, const char *slice_format,
                           void *null);

  void *(*yield_slice)(const void *generator, void *null);
};

struct laud_base {

  const struct Ubject _;
};

#define LAUD_BASE_PRIORITY (UBJECT_PRIORITY + 3)

extern const void *LaudBase;
extern const void *LaudBaseClass;

#endif
