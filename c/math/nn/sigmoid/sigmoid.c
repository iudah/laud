#include <Ubject.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../../core/node.r.static.h"
#include "../../../core/var.r.static.h"
#include "sigmoid.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_sigmoid(struct laud_sigmoid *sigmoid);

static void *differentiate_sigmoid(struct laud_sigmoid *sigmoid,
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

const void *LaudSigmoid = NULL;
const void *LaudSigmoidClass = NULL;

static void fini_sigm();

static void __attribute__((constructor(LAUD_SIGMOID_PRIORITY)))
library_initializer(void) {
  if (!LaudSigmoidClass) {
    LaudSigmoidClass = LaudVarClass;
  }
  if (!LaudSigmoid) {
    LaudSigmoid = init(LaudSigmoidClass, LaudVar,
                       sizeof(struct laud_sigmoid), // class parent size
                       className, "LaudSigmoid",    // class name
                       laud_evaluate_var_node, solve_sigmoid, // evaluate_node
                       laud_differentiate_var_node,
                       differentiate_sigmoid, // differentiate_node
                       NULL);
  }

  atexit(fini_sigm);
}

static void fini_sigm() { FREE(LaudSigmoid); }

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_sigmoid(void *operand) {

  const struct laud_base_class *class = classOf(operand);

  return class->sigmoid(operand, NULL);
}

static void *solve_sigmoid(struct laud_sigmoid *sigmoid) {
  return laud_sigmoid(narray((struct laud_var *)incoming_nodes(sigmoid)[0]));
}

static void *differentiate_sigmoid(struct laud_sigmoid *sigmoid,
                                   const uint64_t operand_index,
                                   const struct laud_narray *derivative) {
  return laud_narray_dsigmoid(
      narray((struct laud_var *)incoming_nodes(sigmoid)[0]), operand_index,
      derivative, narray((struct laud_var *)sigmoid));
}