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
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_binary_cross_entropy(void *operand_a, void *operand_b) {
  const struct laud_base_class *class = classOf(operand_a);
  return class->binary_cross_entropy(operand_a, operand_b, NULL);
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