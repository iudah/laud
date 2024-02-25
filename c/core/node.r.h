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

  size_t outgoing_count;
  size_t outgoing_capacity;
  unsigned char is_visited : 2;
};

#define LAUD_NODE_PRIORITY (LAUD_BASE_PRIORITY + 3)

extern const void *LaudNodeClass;
extern const void *LaudNode;

#ifdef NODE_PROTECTED

#include <stdlib.h>
static void do_depth_first_traversal(struct laud_node *node,
                                     struct laud_node ***nodes,
                                     size_t *capacity, size_t *count);

static inline struct laud_node **outgoing_nodes(void *node) {
  return ((struct laud_node *)node)->outgoing;
}

static inline struct laud_node **incoming_nodes(void *node) {
  return ((struct laud_node *)node)->incoming;
}

static inline void
set_incoming_node_array(struct laud_node *node,
                        struct laud_node **independent_vars) {
  if (node->incoming) {
    UbjectError.error(
        "incoming (source) nodes have been set already. Use append instead");
  }
  node->incoming = independent_vars;
}

static inline size_t compute_position(void *first, void *last, void *current,
                                      size_t count) {
  return count * (current - first) / (last - first);
}

static inline void insert_node(struct laud_node ***node_array,
                               struct laud_node *incoming_node, size_t *count,
                               size_t *capacity) {
  if (!(*node_array)) {
    *capacity = 2;
    *count = 0;

    (*node_array) = malloc(*capacity * sizeof(struct laud_node **));

    (*node_array)[*count] = incoming_node;
    reference(incoming_node);
    (*node_array)[++*count] = NULL;

    return;
  }

  if ((*capacity - 1) <= *count) {
    *capacity *= 2;

    struct laud_node **tmp =
        realloc((*node_array), (*capacity) * sizeof(struct laud_node **));

    if (!tmp) {
      UbjectError.error("unable to allocate memory for node array");
    }

    (*node_array) = tmp;
  }

  if (*count == 1) {

    if ((*node_array)[0] <= incoming_node) {
      (*node_array)[1] = incoming_node;

    } else {
      (*node_array)[1] = (*node_array)[0];
      (*node_array)[0] = incoming_node;
    }

    reference(incoming_node);

    (*node_array)[++*count] = NULL;

    return;
  }

  const size_t position = compute_position(
      (*node_array)[0], (*node_array)[*count], incoming_node, *count);

  if (position < 0) {
    UbjectError.error("position is negative: %i", position);
  }

  ++*count;

  memmove(&(*node_array)[position + 1], &(*node_array)[position],
          *count * sizeof(void *));

  (*node_array)[position] = incoming_node;
}

static inline void **depth_first_traverse(struct laud_node *node) {
  size_t capacity = 2;
  size_t count = 0;
  void **nodes = malloc(capacity * sizeof(struct laud_node *));
  do_depth_first_traversal(node, (struct laud_node ***)&nodes, &capacity,
                           &count);

  void **tmp = realloc(nodes, sizeof(struct laud_node *) * (count + 1));
  if (tmp) {
    nodes = tmp;
  } else {
    UbjectError.error("not enough memory to do travsersal");
  }

  UbjectError.warn("%zu nodes", count);

  nodes[count] = NULL;

  return nodes;
}

static void do_depth_first_traversal(struct laud_node *node,
                                     struct laud_node ***nodes,
                                     size_t *capacity, size_t *count) {

  struct laud_node **dependencies = node->incoming;
  while (dependencies && *dependencies) {

    if (!(*dependencies)->is_visited) {
      do_depth_first_traversal(*dependencies, nodes, capacity, count);
    }

    dependencies++;
  }

  // append dependency
  if (*capacity <= *count) {
    *capacity *= 2;

    void *tmp = realloc(*nodes, sizeof(struct laud_node *) * *capacity);
    if (tmp) {
      *nodes = tmp;
    } else {
      UbjectError.error("not enough memory to do travsersal");
    }
  }

  (*nodes)[(*count)++] = node;
  node->is_visited = 1;

  UbjectError.warn("- - - - %p", node);

  return;
}
#endif

#endif
