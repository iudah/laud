#ifndef LAUD_NODE_H
#define LAUD_NODE_H

#include "base.h"
#include <stdint.h>

LAUDAPI void laud_replace_independent_node(void *var_node, uint64_t index,
                                           void *independent_var);
LAUDAPI uint64_t laud_serialize_graph(void *var_node, const char *fpath);

#endif