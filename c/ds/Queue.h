/**
 * @file Queue.h
 * @brief Declaration of a Queue data structure and associated functions.
 */
#ifndef QUEUE_H
#define QUEUE_H

struct LaudQueue;
/**
 * @brief Structure containing function pointers for Queue operations.
 */
extern const struct LaudQueueFn {
  /**
   * @brief Create a new Queue with the specified capacity.
   *
   * @param count The initial capacity of the Queue.
   * @return A pointer to the created Queue.
   */
  void *(*Queue)(int count);
  /**
   * @brief Enqueue an element into the Queue.
   *
   * @param q A pointer to the Queue.
   * @param x The element to enqueue.
   */
  void (*enqueue)(struct LaudQueue *q, const void *x);
  /**
   * @brief Dequeue an element from the Queue.
   *
   * @param q A pointer to the Queue.
   * @return A pointer to the dequeued element.
   */
  void *(*dequeue)(struct LaudQueue *q);
  /**
   * @brief Get an array representation of the Queue.
   *
   * @param q A pointer to the Queue.
   * @param dest A pointer to an array where the
   * elements will be copied.
   * @return The number of elements in the Queue.
   */
  int (*array)(struct LaudQueue *q, void ***dest);
  /**
   * @brief Get the number of elements in the Queue.
   *
   * @param q A pointer to the Queue.
   * @return The number of elements in the Queue.
   */
  int (*count)(const struct LaudQueue *stack);
} LaudQueueFn;
#endif