/**
 * @file ds/Stack.c
 *
 * This file contains the implementation of the LaudStack structure and related
 * functions.
 */
#include <stdlib.h>

#include "../../../Ubject/Ubject.h"
#include "Stack.h"
/**
 * Structure representing a stack for storing objects.
 */
struct LaudStack {
  void **dynamic_array; /**< Dynamic array to store values. */
  int capacity;         /**< The current capacity of the stack. */
  int count;            /**< The number of elements in the stack. */
};
/**
 * @brief Create a LaudStack object with the specified capacity.
 *
 * This function initializes and returns a LaudStack object with the specified
 * initial capacity.
 *
 * @param count The initial capacity of the LaudStack.
 * @return A pointer to the created LaudStack object.
 */
static void *Stack(int count) {
  struct LaudStack *stack = malloc(sizeof(struct LaudStack));
  stack->capacity = count;
  stack->count = 0;
  stack->dynamic_array = malloc(count * sizeof(void *));
  if (!stack->dynamic_array)
    UbjectError.error("Out of memory");
  return stack;
}
/**
 * @brief Delete a LaudStack object.
 *
 * This function deallocates memory and resources associated with a LaudStack
 * object.
 *
 * @param stack A pointer to the LaudStack to be deleted.
 */
static void del(struct LaudStack *stack) {
  stack->capacity = stack->count = 0;
  free(stack->dynamic_array);
  free(stack);
}
/**
 * @brief Push an object onto the LaudStack.
 *
 * This function pushes an object onto the LaudStack.
 *
 * @param stack A pointer to the LaudStack.
 * @param x A pointer to the object to be pushed.
 */
static void push(struct LaudStack *stack, const void *x) {
  if (stack->capacity == stack->count) // dynamic array
  {
    stack->capacity = stack->capacity ? stack->capacity * 2 : 5;
    stack->dynamic_array =
        realloc(stack->dynamic_array, stack->capacity * sizeof(void *));
  }
  stack->dynamic_array[stack->count] = (void *)x;
  // reference((void *)x);
  stack->count++;
}

/**
 * @brief Pop and remove an object from the LaudStack.
 *
 * This function pops an object from the LaudStack and removes it from the
 * stack.
 *
 * @param stack A pointer to the LaudStack.
 * @return A pointer to the popped object.
 */
static void *pop(struct LaudStack *stack) {
  if (!stack->count)
    // empty stack?
    return NULL;

  void *node = stack->dynamic_array[--stack->count];
  // blip(node);

  return node;
}
/**
 * @brief Get the count of objects in the LaudStack.
 *
 * This function returns the number of objects currently stored in the
 * LaudStack.
 *
 * @param stack A pointer to the LaudStack.
 * @return The count of objects in the stack.
 */
static int count(const struct LaudStack *stack) { return stack->count; }
/**
 * @brief Peek at an object in the LaudStack at a specific index.
 *
 * This function allows you to access an object at a specific index in the
 * LaudStack without removing it.
 *
 * @param stack A pointer to the LaudStack.
 * @param i The index of the object to peek at.
 * @return A pointer to the object at the specified index.
 */
static void *peek(const struct LaudStack *stack, int i) {
  return stack->dynamic_array[i];
}

/**
 * @brief LaudStack function pointers.
 */
const struct LaudStackFn LaudStackFn = {.Stack = Stack,
                                        .del = del,
                                        .push = push,
                                        .pop = pop,
                                        .count = count,
                                        .peek = peek};