#include <Ubject.h>
#include <stdint.h>

#define NODE_PROTECTED
#define VAR_PROTECTED
#include "user_elementary_fn.h"
#include "user_elementary_fn.r.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *laud_user_efn_ctor(void *self, va_list *args);

static void *
solve_user_elementary_fn(struct laud_user_elementary_fn *user_elementary_fn);

static void *differentiate_user_elementary_fn(
    struct laud_user_elementary_fn *user_elementary_fn, uint64_t operand_index,
    const struct laud_narray *derivative);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudUserElementaryFn = NULL;
const void *LaudUserElementaryFnClass = NULL;

static void __attribute__((constructor(LAUD_USER_ELEMENTARY_FN_PRIORITY)))
library_initializer(void) {
  if (!LaudUserElementaryFnClass) {
    LaudUserElementaryFnClass = LaudVarClass;
  }
  if (!LaudUserElementaryFn) {
    LaudUserElementaryFn =
        init(LaudUserElementaryFnClass, LaudVar,
             sizeof(struct laud_user_elementary_fn), // class parent size
             className, "LaudUserElementaryFn",      // class name
             ctor, laud_user_efn_ctor,               //
             laud_evaluate_var_node,
             solve_user_elementary_fn, // evaluate_node
             laud_differentiate_var_node,
             differentiate_user_elementary_fn, // differentiate_node
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

static void *laud_user_efn_ctor(void *self, va_list *args) {

  struct laud_user_elementary_fn *userfn = self;
  userfn->user_elementary_fn = va_arg(*args, laud_user_elementary_fn_t);
  userfn->no_of_operands = va_arg(*args, uint64_t);
  void **operands = va_arg(*args, void **);

  struct laud_node *independent_var = NULL;
  if (operands) {

    // nodes do not have a means to keep track of their incoming node
    // because this information is only required during their creation
    // and never used during destructing or anywhere else so we keep
    // track of the incoming nodes here.
    uint64_t count = 0;
    uint64_t capacity = 0;

    struct laud_node *dependent_var = self;
    uint64_t i = 0;
    while (i < userfn->no_of_operands) {
      independent_var = operands[i];
      // connect nodes dependent_var and independent_var
      // dependent_var <<== independent_var
      insert_node(&dependent_var->incoming, independent_var, &count, &capacity,
                  0);

      // independent_var ==>> dependent_var
      insert_node(&independent_var->outgoing, dependent_var,
                  &independent_var->outgoing_count,
                  &independent_var->outgoing_capacity, 1);

      i++;
    }
  }
  return self;
}

void *laud_user_elementary_fn(laud_user_elementary_fn_t user_elementary_fn,
                              uint64_t no_of_operands, void **operands,
                              void *args) {
  const struct laud_base_class *class = classOf(*operands);
  return class->user_elementary_fn(user_elementary_fn, no_of_operands, operands,
                                   args, NULL);
}

static void *
solve_user_elementary_fn(struct laud_user_elementary_fn *user_elementary_fn) {
  void *operands[user_elementary_fn->no_of_operands];
  for (uint64_t i = 0; i < user_elementary_fn->no_of_operands; i++) {
    operands[i] =
        narray((struct laud_var *)incoming_nodes(user_elementary_fn)[i]);
  }
  return laud_user_elementary_fn(user_elementary_fn->user_elementary_fn,
                                 user_elementary_fn->no_of_operands, operands,
                                 user_elementary_fn->args);
}

static void *differentiate_user_elementary_fn(
    struct laud_user_elementary_fn *user_elementary_fn, uint64_t operand_index,
    const struct laud_narray *derivative) {
  UbjectError.error("Cannot differentiate user function at the moment");
  return laud_narray_duser_elementary_fn(
      narray((struct laud_var *)incoming_nodes(user_elementary_fn)[0]),
      narray((struct laud_var *)incoming_nodes(user_elementary_fn)[1]),
      operand_index, derivative, narray((void *)user_elementary_fn));
}
