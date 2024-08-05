#ifndef NODE_R_STATIC_H
#define NODE_R_STATIC_H

#include <Ubject.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "node.r.h"

static void do_depth_first_traversal(struct laud_node *node,
                                     struct laud_node ***nodes,
                                     uint64_t *capacity, uint64_t *count);

static inline struct laud_node **outgoing_nodes(const void *node) {
  return ((struct laud_node *)node)->outgoing;
}

static inline struct laud_node **incoming_nodes(const void *node) {
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

static inline int64_t compute_position(void *first, void *last, void *current,
                                       uint64_t last_index) {
  return ((int64_t)last_index) * (current - first) / (last - first);
}

static inline void insert_node(struct laud_node ***node_array,
                               struct laud_node *incoming_node, uint64_t *count,
                               uint64_t *capacity, char sort_node) {
  if (!(*node_array)) {
    *capacity = 2;
    *count = 0;

    (*node_array) = CALLOC(*capacity, sizeof(struct laud_node **));

    (*node_array)[*count] = incoming_node;
    // reference(incoming_node);
    (*node_array)[++*count] = NULL;

    return;
  }

  if ((*capacity - 1) <= *count) {
    *capacity *= 2;

    struct laud_node **tmp =
        REALLOC((*node_array), (*capacity) * sizeof(struct laud_node **));

    if (!tmp) {
      UbjectError.error("unable to allocate memory for node array");
    }

    (*node_array) = tmp;
  }

  if (!sort_node) {

    // reference(incoming_node);

    (*node_array)[*count] = incoming_node;
    ++*count;
    (*node_array)[*count] = NULL;
  } else {
    if (*count == 1) {

      if ((*node_array)[0] < incoming_node) {
        (*node_array)[1] = incoming_node;
      } else if ((*node_array)[0] > incoming_node) {
        (*node_array)[1] = (*node_array)[0];
        (*node_array)[0] = incoming_node;
      } else {
        return;
      }

      // reference(incoming_node);

      (*node_array)[++*count] = NULL;

      return;
    }

    int64_t n_left = 0;
    int64_t n_right = *count - 1;
    void *p_left = (*node_array)[n_left];
    void *p_right = (*node_array)[n_right];

    if (((void *)incoming_node) == p_left) {
      return;
    } else if (((void *)incoming_node) == p_right) {
      return;
    } else if (((void *)incoming_node) < p_left) {
      for (uint64_t i = *count; i > 0; i--) {
        (*node_array)[i] = (*node_array)[i - 1];
      }
      (*node_array)[0] = incoming_node;
    } else {
      abort();
    }

    const int64_t position = compute_position(
        (*node_array)[0], (*node_array)[*count - 1], incoming_node, *count - 1);

    if (position < 0 || position > (int64_t)*count) {
      UbjectError.error("position is negative: %" PRId64 "(?)", position);
    }
    if ((*node_array)[position] == incoming_node) {
      return;
    }

    ++*count;

    memmove(&(*node_array)[position + 1], &(*node_array)[position],
            *count * sizeof(void *));

    (*node_array)[position] = incoming_node;
  }
}

static inline void **depth_first_traverse(struct laud_node *node) {
  uint64_t capacity = 2;
  uint64_t count = 0;
  void **nodes = CALLOC(capacity, sizeof(struct laud_node *));
  do_depth_first_traversal(node, (struct laud_node ***)&nodes, &capacity,
                           &count);

  void **tmp = REALLOC(nodes, sizeof(struct laud_node *) * (count + 1));
  if (tmp) {
    nodes = tmp;
  } else {
    UbjectError.error("not enough memory to do travsersal");
  }

  nodes[count] = NULL;

  return nodes;
}

static void do_depth_first_traversal(struct laud_node *node,
                                     struct laud_node ***nodes,
                                     uint64_t *capacity, uint64_t *count) {

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

    void *tmp = REALLOC(*nodes, sizeof(struct laud_node *) * *capacity);
    if (tmp) {
      *nodes = tmp;
    } else {
      UbjectError.error("not enough memory to do travsersal");
    }
  }

  (*nodes)[(*count)++] = node;
  node->is_visited = 1;

  return;
}
#endif
