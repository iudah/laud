#include <Ubject.h>
#include <stdio.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../../core/node.r.static.h"
#include "../../../core/var.r.static.h"
#include "../../../math/common/matrix_dot/matrix_dot.h"
#include "../../../math/common/matrix_dot/matrix_dot.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_matrix_dot(struct laud_matrix_dot *matrix_dot);
static void *differentiate_matrix_dot(struct laud_matrix_dot *matrix_dot,
                                      const uint64_t operand_index,
                                      const struct laud_narray *derivative);
#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

static void finish_lib();

const void *LaudMatrixDot = NULL;
const void *LaudMatrixDotClass = NULL;

static void __attribute__((constructor(LAUD_MATRIX_DOT_PRIORITY)))
library_initializer(void) {
  if (!LaudMatrixDotClass) {
    LaudMatrixDotClass = LaudVarClass;
  }
  if (!LaudMatrixDot) {
    LaudMatrixDot =
        init(LaudMatrixDotClass, LaudVar,
             sizeof(struct laud_matrix_dot),           // class parent size
             className, "LaudMatrixDot",               // class name
             laud_evaluate_var_node, solve_matrix_dot, // evaluate_node
             laud_differentiate_var_node,
             differentiate_matrix_dot, // differentiate_node
             //  ctor, laud_matrix_dot_ctor,        // construtor
             //  dtor, laud_matrix_dot_dtor,        // destrutor
             NULL);
  }

  atexit(finish_lib);
}

static void finish_lib() { FREE(LaudMatrixDot); }

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

static void *solve_matrix_dot(struct laud_matrix_dot *matrix_dot) {
  return laud_matrix_dot(
      narray((struct laud_var *)incoming_nodes(matrix_dot)[0]),
      narray((struct laud_var *)incoming_nodes(matrix_dot)[1]));
}

static void *differentiate_matrix_dot(struct laud_matrix_dot *matrix_dot,
                                      uint64_t operand_index,
                                      const struct laud_narray *derivative) {
  return laud_narray_dmatrix_dot(
      narray((struct laud_var *)incoming_nodes(matrix_dot)[0]),
      narray((struct laud_var *)incoming_nodes(matrix_dot)[1]), operand_index,
      derivative, narray((const void *)matrix_dot));
}

void *laud_matrix_dot(void *a, void *b) {
  const struct laud_base_class *class = classOf(a);

  return class->matrix_dot(a, b, NULL);
}
