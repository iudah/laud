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

#include "../../../Ubject/Ubject.h"
#include "HashMap.h"

struct LaudHashMap {
  void **dynamic_array; /**< Dynamic array to store key-value pairs. */

  int capacity,     /**< Capacity of the array. */
      count,        /**< Number of key-value pairs in the array. */
      iter_current, /**< Current iterator position. */
      iter_count;   /**< Number of iterated elements. */
};

/**
 * @brief Check if a number is prime.
 *
 * @param number The number to check.
 * @return 1 if the number is prime, 0 otherwise.
 */
static char isPrime(int number) {
  if (number <= 1)
    return 0;
  int factor = (int)ceil(sqrt(number));
  while (factor > 2) {
    if (number % factor == 0)
      return 0;
    factor--;
  }
  return 1;
}

/**
 * @brief Find the next prime number larger than the given number.
 *
 * @param number The starting number.
 * @return The next prime number.
 */
static int makePrime(int number) {
  if (number % 2 == 0)
    number++;
  while (!isPrime(number)) {
    number += 2;
  }
  return number;
}

/**
 * @brief Create a new HashMap with the specified capacity.
 *
 * @param count The initial capacity of the HashMap.
 * @return A pointer to the created HashMap.
 */
static void *HashMap(int count) {
  struct LaudHashMap *map = malloc(sizeof(struct LaudHashMap));
  count = makePrime(count);
  map->capacity = count;
  map->count = 0;
  map->dynamic_array = calloc(2 * count, sizeof(void *));
  return map;
}

/**
 * @brief Get the hash value for a key.
 *
 * @param x The key.
 * @param map_ The HashMap.
 * @return The hash value.
 */
static int get_hash(const void *x, const void *map_) {
  size_t diff = (void *)get_hash - x;
  return diff % ((struct LaudHashMap *)map_)->capacity;
}

/**
 * @brief Get the next hash value for a key, used for collision resolution.
 *
 * @param hash The original hash value.
 * @param i The collision resolution index.
 * @param map_ The HashMap.
 * @return The next hash value.
 */
static int get_next_hash(int hash, int i, const void *map_) {
  return (hash * hash + i * hash + i * i) %
         ((struct LaudHashMap *)map_)->capacity;
}

/**
 * @brief Find a key-value pair in the HashMap using its key and compute its
 * hash.
 *
 * @param map The HashMap.
 * @param key The key to search for.
 * @param hash The computed hash value.
 * @return A pointer to the holder of the key-value pair.
 */
static void *find_key_hash_match(struct LaudHashMap *map, const void *key,
                                 int *hash) {
  *hash = get_hash(key, map);
  const void **holder = (const void **)map->dynamic_array + 2 * *hash;

  int i = 0;
  while (*holder) {
    if (*holder == key) {
      break;
    }
    holder = (const void **)map->dynamic_array +
             (*hash = 2 * get_next_hash(*hash, ++i, map));
  }
  return holder;
}

/**
 * @brief Find a value in the HashMap using its key and hash.
 *
 * @param map The HashMap.
 * @param key The key to search for.
 * @param hash The computed hash value.
 * @return A pointer to the value or NULL if not found.
 */
static const void *find(void *map, void *key, int *hash) {
  void **holder = find_key_hash_match(map, key, hash ? hash : (int *)&hash);
  return holder[0] == key ? holder[1] : NULL;
}

/**
 * @brief Enlarge the capacity of the HashMap when it becomes full.
 *
 * @param map_ A pointer to the HashMap.
 * @return A pointer to the enlarged dynamic array.
 */
static void *enlarge_map(struct LaudHashMap *map_) {
  int capacity = makePrime(map_->capacity + 2);
  struct LaudHashMap new_map = {calloc(2 * capacity, sizeof(void *)), capacity,
                                0};
  for (int i = 0; i < map_->capacity; i++) {
    LaudHashMapFn.insert(&new_map, map_->dynamic_array[i * 2],
                         map_->dynamic_array[i * 2 + 1]);
  }
  free(map_->dynamic_array);
  return map_->dynamic_array = new_map.dynamic_array;
}
/**
 * @brief Place a key-value pair into the HashMap.
 *
 * This function is used to insert a key-value pair into the HashMap.
 *
 * @param map_ A pointer to the HashMap.
 * @param key The key to insert.
 * @param value The corresponding value to insert.
 * @param replaced If not NULL and the key already exists, this flag is set
 * to 1.
 * @return The value associated with the inserted key or the new value if
 * replaced.
 */
static const void *place(void *map_, const void *key, const void *value,
                         char *replaced) {

  struct LaudHashMap *map = map_;
  if (map->capacity == map->count) {
    map = enlarge_map(map_);
  }

  int hash;
  const void **holder = find_key_hash_match(map_, key, &hash);
  if (*holder == key && !replaced) {
    UbjectError.warn("LaudHashMap: key already exists\n");
    return holder[1];
  }

  if (replaced)
    *replaced = 1;

  ((struct LaudHashMap *)map)->count++;
  *holder = key;
  return *(holder + 1) = value;
}

/**
 * @brief Insert a key-value pair into the HashMap.
 *
 * This function inserts a key-value pair into the HashMap. If the key already
 * exists, it does not replace it.
 *
 * @param map_ A pointer to the HashMap.
 * @param key The key to insert.
 * @param value The corresponding value to insert.
 * @return The value associated with the inserted key or NULL if the key exists.
 */
static const void *insert(void *map_, const void *key, const void *value) {
  return place(map_, key, value, NULL);
}
/**
 * @brief Insert a key-value pair into the HashMap or replace an existing key's
 * value.
 *
 * This function inserts a key-value pair into the HashMap or replaces the value
 * of an existing key.
 *
 * @param map_ A pointer to the HashMap.
 * @param key The key to insert or replace.
 * @param value The corresponding value to insert or replace.
 * @param replaced If not NULL and the key already exists, this flag is set
 * to 1.
 * @return The value associated with the inserted key or the new value if
 * replaced.
 */
static const void *insert_or_replace(void *map_, void *key, void *value,
                                     char *replaced) {
  return place(map_, key, value, replaced ? replaced : (char *)&replaced);
}

/**
 * @brief Replace a key's value using its hash value.
 *
 * This function replaces a key's value using its hash value. It is used when
 * the key and hash are known to correspond.
 *
 * @param map_ A pointer to the HashMap.
 * @param key The key to replace.
 * @param value The new value to set.
 * @param hash The hash value of the key.
 * @return The previous value associated with the key.
 */
static const void *replace_key_with_hash(void *map_, void *key, void *value,
                                         int hash) {
  struct LaudHashMap *map = map_;
  void *Value;
  if (map->dynamic_array[hash * 2] != key) {
    UbjectError.error(
        "replace_key_with_hash: key and hash do not correspond\n");
  }

  Value = map->dynamic_array[hash * 2 + 1];
  map->dynamic_array[hash * 2 + 1] = value;
  return Value;
}
/**
 * @brief Get the number of key-value pairs in the HashMap.
 *
 * @param map_ A pointer to the HashMap.
 * @return The number of key-value pairs in the HashMap.
 */
static const int count(void *map_) {
  return ((struct LaudHashMap *)map_)->count;
}

/**
 * @brief Delete the HashMap and its associated resources.
 *
 * @param map_ A pointer to the HashMap to delete.
 */
static void del_hashmap(void *map_) {
  struct LaudHashMap *map = map_;
  free(map->dynamic_array);
  free(map);
  return;
}

/**
 * @brief Initialize the iterator to the beginning of the HashMap.
 *
 * @param map_ A pointer to the HashMap.
 */
static void iter_begin(void *map_) {
  struct LaudHashMap *map = map_;
  map->iter_current = map->iter_count = 0;
  return;
}

/**
 * @brief Get the next key-value pair during iteration.
 *
 * @param map_ A pointer to the HashMap.
 * @return A pointer to the next key-value pair or NULL if the end of the
 * HashMap is reached.
 */
static void **iter_next(void *map_) {
  struct LaudHashMap *map = map_;
  if (map->iter_count == map->count)
    return NULL;
  while (!map->dynamic_array[map->iter_current]) {
    map->iter_current += 2;
  }
  map->iter_count++;
  map->iter_current += 2;
  return map->dynamic_array + ((map->iter_current - 2));
}
const struct LaudHashMapFn LaudHashMapFn = {
    .HashMap = HashMap,
    .find = find,
    .insert = insert,
    .insert_or_replace = insert_or_replace,
    .replace_key_using_hash = replace_key_with_hash,
    .count = count,
    .del = del_hashmap,
    .iter_begin = iter_begin,
    .iter_next = iter_next};