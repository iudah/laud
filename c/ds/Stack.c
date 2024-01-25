/**
 * @file ds/Stack.c
 *
 * This file contains the implementation of the LaudStack structure and related
 * functions.
 */
#include <stdlib.h>

#include <Ubject.h>

#include "Stack.h"

struct LaudStack {
  void **elements;       /**< Dynamic array to store values. */
  size_t capacity;       /**< The current capacity of the stack. */
  size_t size;           /**< The number of elements in the stack. */
  size_t iterator_index; /**< Index used for iteration. */
};

static void *create_stack(int initial_capacity) {
  struct LaudStack *stack = malloc(sizeof(struct LaudStack));
  if (!stack) {
    UbjectError.error("create_stack: Out of memory");
  }

  stack->capacity = initial_capacity;
  stack->size = 0;
  stack->elements = malloc(initial_capacity * sizeof(void *));
  if (!stack->elements)
    UbjectError.error("create_stack: Out of memory");
  stack->iterator_index = (size_t)-1;
  return stack;
}

static void delete_stack(struct LaudStack *stack) {
  if (stack) {
    free(stack->elements);
    stack->elements = NULL;
    stack->capacity = stack->size = 0;
    free(stack);
    stack = NULL;
  }
}

static void push(struct LaudStack *stack, const void *element) {
  if (stack->capacity == stack->size) // dynamic array
  {
    size_t new_capacity = stack->capacity ? stack->capacity * 2 : 5;
    void **new_elements =
        realloc(stack->elements, new_capacity * sizeof(void *));

    if (!new_elements) {
      UbjectError.error("push: Out of memory");
      return;
    }

    stack->elements = new_elements;
    stack->capacity = new_capacity;
  }

  stack->elements[stack->size] = (void *)element;
  stack->size++;
}

static void *pop(struct LaudStack *stack) {
  if (stack->size == 0) {
    // Stack is empty
    return NULL;
  }

  void *node = stack->elements[--stack->size];

  return node;
}

static int count(const struct LaudStack *stack) { return stack->size; }

static void *peek(const struct LaudStack *stack, size_t i) {
  if (i < stack->size) {
    return stack->elements[i];
  } else {
    // Handle out-of-bounds index (e.g., return an error or print a message)
    return NULL;
  }
}

static size_t iter_begin(struct LaudStack *stack) {
  stack->iterator_index = 0;
  return stack->size;
}
static void *iter_next(struct LaudStack *stack) {
  if (stack->iterator_index < stack->size) {
    return peek(stack, stack->iterator_index++);
  } else {
    return NULL; // End of iteration
  }
}

static void iter_end(struct LaudStack *self) {
  struct LaudStack *stack = (void *)self;
  stack->iterator_index = -1;
}

void *replace_item(struct LaudStack *self, void *item, size_t array_index) {
  struct LaudStack *this = (void *)self;
  void *return_value = NULL;

  // Check for NULL item
  if (item == NULL) {
    UbjectError.error("replace_item: Item cannot be NULL");
    return NULL;
  }

  // Check if array_index is within bounds
  if (array_index < this->capacity) {
    return_value = this->elements[array_index];
    this->elements[array_index] = item;
  } else {
    UbjectError.error("replace_item: Index out of bounds");
  }
  return return_value;
}

/**
 * @brief LaudStack function pointers.
 */
const struct LaudStackFn LaudStackFn = {.Stack = create_stack,
                                        .del = delete_stack,
                                        .push = push,
                                        .pop = pop,
                                        .count = count,
                                        .peek = peek,
                                        .iter_start = iter_begin,
                                        .yield = iter_next,
                                        .iter_end = iter_end,
                                        .replace_item = replace_item};
