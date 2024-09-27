#ifndef LAUD_USER_ELEMENTARY_FN_DEF_H
#define LAUD_USER_ELEMENTARY_FN_DEF_H

#include <stdint.h>

typedef void (*laud_get_bc_value_fn_t)(uint64_t offset, number_t *values);

typedef void (*laud_user_elementary_fn_t)(number_t *result,
                                          uint64_t result_length,
                                          number_t **operands_vals,
                                          laud_get_bc_value_fn_t get_bc_value,
                                          void *args);

#endif