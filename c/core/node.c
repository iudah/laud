#include <Ubject.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NODE_PROTECTED
#include "node.h"
#include "node.r.static.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Static function declarations
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_FUNC_DECL

static void *laud_node_dtor(struct laud_node *self);

#undef STATIC_FUNC_DECL

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class Initializer
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLASS_INIT

const void *LaudNodeClass = NULL;
const void *LaudNode = NULL;

static void finish_lib() { FREE((void *)LaudNode); }

static void __attribute__((constructor(LAUD_NODE_PRIORITY)))
library_initializer(void) {

  if (!LaudNodeClass) {
    LaudNodeClass = LaudBaseClass;
  }

  if (!LaudNode) {
    LaudNode =
        init(LaudNodeClass, LaudBase, sizeof(struct laud_node), className,
             "LaudNode",           // class name
             dtor, laud_node_dtor, // destructor
             NULL);
  }

  atexit(finish_lib);
}

#undef CLASS_INIT

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Implemention
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPL

static void *laud_node_dtor(struct laud_node *self) {
  uint64_t i = 0;

  if (self->incoming) {
    while (self->incoming[i]) {
      blip(self->incoming[i++]);
    }
    FREE(self->incoming);
    self->incoming = NULL;
  }

  i = 0;
  if (self->outgoing) {
    /*while (self->outgoing[i]) {self->outgoing[i++]=NULL;
     // blip(self->outgoing[i++]);
    }*/
    FREE(self->outgoing);
    self->outgoing = NULL;
  }

  return super_dtor(LaudNode, self);
}

void laud_replace_independent_node(void *var_node, uint64_t index,
                                   void *independent_node) {
  struct laud_node *y_node = var_node;
  struct laud_node *new_node = independent_node;
  struct laud_node *old_node = y_node->incoming[index];

  if (new_node == old_node)
    return;

  y_node->incoming[index] = new_node;

  blip(old_node);
  reference(new_node);

  insert_node(&new_node->outgoing, y_node, &new_node->outgoing_count,
              &new_node->outgoing_capacity, 1);

  struct laud_node **node_array = old_node->outgoing;
  uint64_t count = old_node->outgoing_count -= 1;
  int64_t n_left = 0;
  int64_t n_right = count;
  void *p_left = node_array[n_left];
  void *p_right = node_array[n_right];

  if (((void *)y_node) == p_left) {
    for (uint64_t i = 0; i <= count; i++) {
      (node_array)[i] = (node_array)[i + 1];
    }
  } else if (((void *)y_node) == p_right) {
    node_array[n_right] = NULL;
  } else {
    abort();
  }
}

 uint64_t laud_serialize_graph(void *var_node,const char *fpath){
     FILE *f = fopen(fpath, "wb");
  // list all dependencies
  struct laud_node **nodes =
      (struct laud_node **)depth_first_traverse((struct laud_node *)var_node);
      
        // for each dependency call evaluate()
  struct laud_node **active_node = nodes;
  uint64_t i=0;
  uint64_t len=0;
  while (active_node && *active_node) {

    len+=puto(*active_node,f);
    (*active_node)->outgoing_capacity=i++;
    
    struct laud_node **deps = (*active_node)->incoming;uint64_t zero =0;
    while(deps && *deps ){
        len+=fwrite(&(*deps)->outgoing_capacity, 1, sizeof((*deps)->outgoing_capacity), f);
        deps++;
    }
        len+=fwrite(&zero, 1, sizeof(zero), f);

    /*
    if ((*active_var)->derivative) {
      blip((*active_var)->derivative);
      (*active_var)->derivative = NULL;
    }
    */

    ((struct laud_node *)*active_node)->is_visited = 0;
    active_node++;
  }

  FREE(nodes);

  return len;
 }
