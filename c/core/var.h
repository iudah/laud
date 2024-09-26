#ifndef LAUD_VAR_H
#define LAUD_VAR_H

#include "base.h"
#include <stdint.h>

LAUDAPI void *laud_var();

LAUDAPI void laud_set_variable_value(void *variable_node, void *value,
                                     void **old_value);

LAUDAPI void laud_unset_variable_value(void *variable_node);

LAUDAPI void *laud_derivative_of(void *var_node);

LAUDAPI void *laud_value(void *var_node);

#endif