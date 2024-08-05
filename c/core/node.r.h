#ifndef NODE_R_H
#define NODE_R_H

#include "base.r.h"

struct laud_node_class {
  struct laud_base_class _;
};

struct laud_node {
  struct laud_base _;

  struct laud_node **outgoing;
  struct laud_node **incoming;

  uint64_t outgoing_count;
  uint64_t outgoing_capacity;
  unsigned char is_visited : 2;
};

#define LAUD_NODE_PRIORITY (LAUD_BASE_PRIORITY + 3)

extern const void *LaudNodeClass;
extern const void *LaudNode;

#endif
