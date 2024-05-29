#ifndef USER_ELEMENTARY_FN_H
#define USER_ELEMENTARY_FN_H

#include <stdint.h>

#include "../../../core/base.h"
#include "../../../math/others/user_elementary_fn/user_elementary_fn.def.h"

LAUDAPI void *
laud_user_elementary_fn(laud_user_elementary_fn_t user_elementary_fn,
                        uint64_t no_of_operands, void **operands, void *args);

#endif