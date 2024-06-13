#include <Ubject.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../../math/nn/binary_cross_entropy/binary_cross_entropy.h"
#include "../../../math/nn/binary_cross_entropy/binary_cross_entropy.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_binary_cross_entropy(
    struct laud_binary_cross_entropy *binary_cross_entropy);

static void *differentiate_binary_cross_entropy(
    struct laud_binary_cross_entropy *binary_cross_entropy,
    uint64_t operand_index, const struct laud_narray *derivative);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT
static void fini_bce();
const void *LaudBinaryCrossEntropy = NULL;
const void *LaudBinaryCrossEntropyClass = NULL;

static void __attribute__((constructor(LAUD_BINARY_CROSS_ENTROPY_PRIORITY)))
library_initializer(void) {
  if (!LaudBinaryCrossEntropyClass) {
    LaudBinaryCrossEntropyClass = LaudVarClass;
  }
  if (!LaudBinaryCrossEntropy) {
    LaudBinaryCrossEntropy = init(
        LaudBinaryCrossEntropyClass, LaudVar,
        sizeof(struct laud_binary_cross_entropy),           // class parent size
        className, "LaudBinaryCrossEntropy",                // class name
        laud_evaluate_var_node, solve_binary_cross_entropy, // evaluate_node
        laud_differentiate_var_node,
        differentiate_binary_cross_entropy, // differentiate_node
        NULL);
  }
atexit(fini_bce);
}

static void fini_bce(){
    FREE(LaudBinaryCrossEntropy);
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_binary_cross_entropy(void *forecast, void *truth) {
  const struct laud_base_class *class = classOf(forecast);
  return class->binary_cross_entropy(forecast, truth, NULL);
}

static void *solve_binary_cross_entropy(
    struct laud_binary_cross_entropy *binary_cross_entropy) {
  return laud_binary_cross_entropy(
      narray((struct laud_var *)incoming_nodes(binary_cross_entropy)[0]),
      narray((struct laud_var *)incoming_nodes(binary_cross_entropy)[1]));
}

static void *differentiate_binary_cross_entropy(
    struct laud_binary_cross_entropy *binary_cross_entropy,
    uint64_t operand_index, const struct laud_narray *derivative) {
  return laud_narray_dbinary_cross_entropy(
      narray((struct laud_var *)incoming_nodes(binary_cross_entropy)[0]),
      narray((struct laud_var *)incoming_nodes(binary_cross_entropy)[1]),
      operand_index, derivative, narray((void *)binary_cross_entropy));
}