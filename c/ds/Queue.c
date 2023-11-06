#include <stdlib.h>
#include <string.h>

#include "Queue.h"

struct LaudQueue {
  void **dynamic_array; /**< Dynamic array to store values. */
  int capacity;         /**< The maximum capacity of the LaudQueue. */
  int count; /**< The number of elements currently in the LaudQueue. */
  int front; /**< The index of the front element in the LaudQueue. */
};
// Define a constant for the initial capacity to make it more configurable.
#define INITIAL_CAPACITY 10
/**
 * @brief Create a new LaudQueue with an initial capacity.
 *
 * @param count The initial capacity of the LaudQueue.
 * @return A pointer to the created LaudQueue.
 */
static void *Queue(int count) {
  struct LaudQueue *q = malloc(sizeof(struct LaudQueue));
  // Use the defined constant for the initial capacity.
  q->capacity = count > 0 ? count : INITIAL_CAPACITY;
  q->count = q->front = 0;
  q->dynamic_array = malloc(count * sizeof(void *));
  return q;
}
/**
 * @brief Enqueue an element into the LaudQueue.
 *
 * @param q A pointer to the LaudQueue.
 * @param x The element to enqueue.
 */
static void enqueue(struct LaudQueue *q, const void *x) {
  if (q->capacity == q->count) // dynamic array
  {                            // If the queue is full, double its capacity.
    q->capacity *= 2;
    q->dynamic_array = realloc(q->dynamic_array, q->capacity * sizeof(void *));
  }
  q->dynamic_array[q->count] = (void *)x;
  // not an Ubject, so leave Ubject stuff to Ubjects
  // reference((void *)x);
  q->count++;
}
/**
 * @brief Dequeue an element from the LaudQueue.
 *
 * @param q A pointer to the LaudQueue.
 * @return A pointer to the dequeued element or NULL if the queue is empty.
 */
static void *dequeue(struct LaudQueue *q) {
  if (!q->count)
    // empty queue?
    return NULL;

  void *node = q->dynamic_array[q->front++];
  q->count--;

  // Consider resizing the dynamic_array if the queue is significantly smaller
  // than its capacity.
  if (q->count > 0 && q->count <= (q->capacity >> 2)) {
    // Adjust the capacity and move elements if necessary.
    if (q->front > 0) {
      memmove(q->dynamic_array, q->dynamic_array + q->front,
              q->count * sizeof(void *));
      q->front = 0;
    }
    q->capacity /= 2;
    q->dynamic_array = realloc(q->dynamic_array, q->capacity * sizeof(void *));
  }

  // Reset the front and count if the queue is empty.
  if (q->count == 0) {
    q->front = 0;
  }

  return node;
}
/**
 * @brief Get the number of elements in the LaudQueue.
 *
 * @param q A pointer to the LaudQueue.
 * @return The number of elements in the LaudQueue.
 */
static int count(const struct LaudQueue *q) { return q->count; }
/**
 * @brief Extract the dynamic array representation of the LaudQueue.
 *
 * @param q A pointer to the LaudQueue.
 * @param dest A pointer to an array of void pointers to hold the elements.
 * @return The number of elements in the LaudQueue.
 */
static int array(struct LaudQueue *q, void ***dest) {
  if (q->capacity == q->count && !q->front)
    *dest = q->dynamic_array;
  else {
    if (q->front) {
      memmove(q->dynamic_array, q->dynamic_array + q->front,
              q->count * sizeof(void *));
      q->front = 0;
    }
  }
  return q->count;
}

const struct LaudQueueFn LaudQueueFn = {.Queue = Queue,
                                        .enqueue = enqueue,
                                        .dequeue = dequeue,
                                        .array = array,
                                        .count = count};