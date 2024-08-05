#include <Ubject.h>
#include <stdlib.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "../../../core/narray.r.h"
#include "../../../core/node.r.static.h"
#include "../../../core/var.r.static.h"
#include "../../../math/common/add/add.h"
#include "../../../math/common/add/add.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *solve_add(struct laud_add *add);
static void *differentiate_add(struct laud_add *add, uint64_t operand_index,
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

const void *LaudAdd = NULL;
const void *LaudAddClass = NULL;

static void __attribute__((constructor(LAUD_ADD_PRIORITY)))
library_initializer(void) {
  if (!LaudAddClass) {
    LaudAddClass = LaudVarClass;
  }
  if (!LaudAdd) {
    LaudAdd =
        init(LaudAddClass, LaudVar,
             sizeof(struct laud_add),           // class parent size
             className, "LaudAdd",              // class name
             laud_evaluate_var_node, solve_add, // evaluate_node
             laud_differentiate_var_node, differentiate_add, // evaluate_node
             NULL);
  }

  atexit(finish_lib);
}

static void finish_lib() { FREE(LaudAdd); }

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

void *laud_add(void *a, void *b) {
  const struct laud_base_class *class = classOf(a);
  return class->add(a, b, NULL);
}

static void *solve_add(struct laud_add *add) {
  return laud_add(narray((struct laud_var *)incoming_nodes(add)[0]),
                  narray((struct laud_var *)incoming_nodes(add)[1]));
}

static void *differentiate_add(struct laud_add *add, uint64_t operand_index,
                               const struct laud_narray *derivative) {
  return laud_narray_dadd(narray((struct laud_var *)incoming_nodes(add)[0]),
                          narray((struct laud_var *)incoming_nodes(add)[1]),
                          operand_index, derivative, narray((void *)add));
}