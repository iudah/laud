#include <Ubject.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "relu.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_relu(struct laud_relu *relu);
static void *differentiate_relu(struct laud_relu *relu,
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

const void *LaudReLU = NULL;
const void *LaudReLUClass = NULL;
static void fini_relu();
static void __attribute__((constructor(LAUD_RELU_PRIORITY)))
library_initializer(void) {
  if (!LaudReLUClass) {
    LaudReLUClass = LaudVarClass;
  }
  if (!LaudReLU) {
    LaudReLU = init(LaudReLUClass, LaudVar,
                    sizeof(struct laud_relu),           // class parent size
                    className, "LaudReLU",              // class name
                    laud_evaluate_var_node, solve_relu, // evaluate_node
                    laud_differentiate_var_node,
                    differentiate_relu, // differentiate_node
                    NULL);
  }
  
  atexit(fini_relu);
}

static void fini_relu(){
    FREE(LaudReLU);
}


#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_relu(void *operand) {

  const struct laud_base_class *class = classOf(operand);

  return class->relu(operand, NULL);
}

static void *solve_relu(struct laud_relu *relu) {
  return laud_relu(narray((struct laud_var *)incoming_nodes(relu)[0]));
}

static void *differentiate_relu(struct laud_relu *relu,
                                const uint64_t operand_index,
                                const struct laud_narray *derivative) {
  return laud_narray_drelu(narray((struct laud_var *)incoming_nodes(relu)[0]),
                           operand_index, derivative,
                           narray((const void *)relu));
}
