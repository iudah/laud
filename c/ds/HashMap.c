/**
 * @file HashMap.c
 * @author Jude
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <Ubject.h>

#include "HashMap.h"

struct LaudHashMap {
  void **dynamic_array; /**< Dynamic array to store key-value pairs. */

  int capacity,     /**< Capacity of the array. */
      count,        /**< Number of key-value pairs in the array. */
      iter_current, /**< Current iterator position. */
      iter_count;   /**< Number of iterated elements. */
};

static char is_prime(int number) {
  if (number <= 1)
    return 0;

  int max_factor = (int)sqrt(number) + 1;

  for (int factor = max_factor; factor > 1; factor--) {
    if (number % factor == 0)
      return 0;
  }

  return 1;
}

static int make_prime(int number) {
  if (number % 2 == 0)
    number++;

  while (!is_prime(number)) {
    number += 2;
  }

  return number;
}

static void *create_hash_map(int initial_capacity) {
  struct LaudHashMap *map = malloc(sizeof(struct LaudHashMap));
  initial_capacity = make_prime(initial_capacity);
  map->capacity = initial_capacity;
  map->count = 0;
  map->iter_count = map->iter_current = -1;
  map->dynamic_array = calloc(2 * initial_capacity, sizeof(void *));
  return map;
}

static int calculate_hash(const void *key, const void *map) {
  size_t key_address_diff = (size_t)key - (size_t)calculate_hash;
  return key_address_diff % ((struct LaudHashMap *)map)->capacity;
}

static int calculate_next_hash(int current_hash, int attempt,
                               const struct LaudHashMap *map) {
  return (current_hash * current_hash + attempt * current_hash +
          attempt * attempt) %
         map->capacity;
}

static void *find_key_hash_match(struct LaudHashMap *map, const void *key,
                                 int *out_hash) {
  *out_hash = calculate_hash(key, map);
  const void **slot = (const void **)map->dynamic_array + 2 * (*out_hash);

  int i = 0;
  while (*slot) {
    if (*slot == key) {
      break;
    }
    slot = (const void **)map->dynamic_array +
           (*out_hash = 2 * calculate_next_hash(*out_hash, ++i, map));
  }
  return slot;
}

static const void *find(void *map, void *key, int *out_hash) {
  int hash_value;
  void **slot =
      find_key_hash_match(map, key, out_hash ? out_hash : &hash_value);

  return (slot[0] == key) ? slot[1] : NULL;
}

static void *enlarge_map(struct LaudHashMap *map) {
  int new_capacity = make_prime(map->capacity + 2);
  struct LaudHashMap new_map = {.dynamic_array =
                                    calloc(2 * new_capacity, sizeof(void *)),
                                .capacity = new_capacity,
                                .count = 0};
  for (int i = 0; i < map->capacity; i++) {
    LaudHashMapFn.insert(&new_map, map->dynamic_array[i * 2],
                         map->dynamic_array[i * 2 + 1]);
  }
  free(map->dynamic_array);
  return map->dynamic_array = new_map.dynamic_array;
}

static const void *insert_entry(void *map_, const void *key, const void *value,
                                char *replaced) {

  struct LaudHashMap *map = map_;
  if (map->capacity == map->count) {
    map = enlarge_map(map_);
  }

  int hash;
  const void **existing_entry = find_key_hash_match(map_, key, &hash);
  if (*existing_entry == key && replaced == NULL) {
    UbjectError.warn("insert_entry: key already exists\n");
    return existing_entry[1];
  }

  if (replaced)
    *replaced = 1;

  map->count++;
  *existing_entry = key;
  return *(existing_entry + 1) = value;
}

static const void *insert(void *map_, const void *key, const void *value) {
  return insert_entry(map_, key, value, NULL);
}

static const void *insert_or_replace(void *map_, void *key, void *value,
                                     char *replaced) {
  return insert_entry(map_, key, value,
                      replaced ? replaced : (char *)&replaced);
}

static const void *replace_key_with_hash(void *map_, void *key, void *value,
                                         int hash) {
  struct LaudHashMap *map = map_;
  void *previous_value;
  if (map->dynamic_array[hash * 2] != key) {
    UbjectError.error(
        "replace_key_with_hash: key and hash do not correspond\n");
  }

  previous_value = map->dynamic_array[hash * 2 + 1];
  map->dynamic_array[hash * 2 + 1] = value;
  return previous_value;
}

static int hashmap_count(void *map_) {
  return ((struct LaudHashMap *)map_)->count;
}

static void hashmap_delete(void *map_) {
  struct LaudHashMap *map = map_;
  free(map->dynamic_array);
  free(map);
  return;
}

static size_t hashmap_iter_begin(void *map_) {
  struct LaudHashMap *map = map_;
  map->iter_current = map->iter_count = 0;
  return map->count;
}

static void **hashmap_iter_next(void *map_) {
  struct LaudHashMap *map = map_;
  if (map->iter_current == -1 || map->iter_count == -1)
    UbjectError.error("%s:%i: iteration not started", __FILE__, __LINE__);
  if (map->iter_count == map->count)
    return NULL;
  while (!map->dynamic_array[map->iter_current]) {
    map->iter_current += 2;
  }
  map->iter_count++;
  map->iter_current += 2;
  return map->dynamic_array + ((map->iter_current - 2));
}

static void hashmap_iter_end(void *map_) {
  struct LaudHashMap *map = map_;
  map->iter_current = map->iter_count = -1;
}

const struct LaudHashMapFn LaudHashMapFn = {
    .create_hash_map = create_hash_map,
    .find = find,
    .insert = insert,
    .insert_or_replace = insert_or_replace,
    .replace_key_using_hash = replace_key_with_hash,
    .count = hashmap_count,
    .delete_hash_map = hashmap_delete,
    .iter_start = hashmap_iter_begin,
    .yield = hashmap_iter_next,
    .iter_end = hashmap_iter_end};
