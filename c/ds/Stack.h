/**
 * @file ds/Stack.h
 *
 * This file contains declarations for the LaudStack structure and related
 * functions.
 */
#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/**
 * @struct LaudStack
 *
 * Structure representing a stack for storing objects.
 */
struct LaudStack;
extern const struct LaudStackFn {
  /**
   * @brief Function to create a LaudStack object with the specified capacity.
   *
   * This function initializes and returns a LaudStack object with the specified
   * initial capacity.
   *
   * @param count The initial capacity of the LaudStack.
   * @return A pointer to the created LaudStack object.
   */
  void *(*Stack)(int count);

  /**
   * @brief Function to delete a LaudStack object.
   *
   * This function deallocates memory and resources associated with a LaudStack
   * object.
   *
   * @param stack A pointer to the LaudStack to be deleted.
   */
  void (*del)(struct LaudStack *stack);

  /**
   * @brief Function to push an object onto the LaudStack.
   *
   * This function pushes an object onto the LaudStack.
   *
   * @param stack A pointer to the LaudStack.
   * @param x A pointer to the object to be pushed.
   */
  void (*push)(struct LaudStack *stack, const void *x);

  /**
   * @brief Function to pop and remove an object from the LaudStack.
   *
   * This function pops an object from the LaudStack and removes it from the
   * stack.
   *
   * @param stack A pointer to the LaudStack.
   * @return A pointer to the popped object.
   */
  void *(*pop)(struct LaudStack *stack);

  /**
   * @brief Function to get the count of objects in the LaudStack.
   *
   * This function returns the number of objects currently stored in the
   * LaudStack.
   *
   * @param stack A pointer to the LaudStack.
   * @return The count of objects in the stack.
   */
  int (*count)(const struct LaudStack *stack);

  /**
   * @brief Function to peek at an object in the LaudStack at a specific index.
   *
   * This function allows you to access an object at a specific index in the
   * LaudStack without removing it.
   *
   * @param stack A pointer to the LaudStack.
   * @param i The index of the object to peek at.
   * @return A pointer to the object at the specified index.
   */
  void *(*peek)(const struct LaudStack *stack, size_t i);

  /**
   * @brief Function to begin the iteration over the LaudStack.
   *
   * @param stack A pointer to the LaudStack.
   * @return The number of elements in the LaudStack.
   */
  size_t (*iter_start)(struct LaudStack *stack);

  /**
   * @brief Function to yield the next element during iteration.
   *
   * @param stack A pointer to the LaudStack.
   * @return A pointer to the next element or NULL if the end of the LaudStack
   * is reached.
   */
  void *(*yield)(struct LaudStack *stack);

  /**
   * @brief Function to end the iteration over the LaudStack.
   *
   * @param stack A pointer to the LaudStack.
   */
  void (*iter_end)(struct LaudStack *stack);

  /**
   * @brief Function to replace an item in the LaudStack at a specific index.
   *
   * This function replaces an item in the LaudStack at the specified index.
   *
   * @param stack A pointer to the LaudStack.
   * @param item A pointer to the new item.
   * @param array_index The index of the item to be replaced.
   * @return A pointer to the item that was replaced or NULL if the index is
   * out of bounds.
   */
  void *(*replace_item)(struct LaudStack *stack, void *item,
                        size_t array_index);
} LaudStackFn;

#endif
