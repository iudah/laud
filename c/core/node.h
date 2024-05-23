#ifndef NODE_H
#define NODE_H

#include "base.h"
#include <stdint.h>

LAUDAPI void laud_replace_independent_node(void *var_node, uint64_t index,
                                           void *independent_var);

#endif