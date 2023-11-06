/**
 * @file HashMap.h
 * @brief Declaration of a HashMap data structure and associated functions.
 */
#ifndef HASHMAP_H
#define HASHMAP_H

struct LaudHashMap;
/**
 * @brief Structure containing function pointers for HashMap operations.
 */
extern const struct LaudHashMapFn {

  /**
   * @brief Create a new HashMap with the specified capacity.
   *
   * @param count The initial capacity of the HashMap.
   * @return A pointer to the created HashMap.
   */
  void *(*HashMap)(int count);
  /**
   * @brief Find a value in the HashMap using its key and hash.
   *
   * @param map The HashMap.
   * @param key The key to search for.
   * @param hash The computed hash value.
   * @return A pointer to the value or NULL if not found.
   */
  const void *(*find)(void *map, void *key, int *hash);

  /**
   * @brief Insert a key-value pair into the HashMap.
   *
   * This function inserts a key-value pair into the HashMap. If the key already
   * exists, it does not replace it.
   *
   * @param map A pointer to the HashMap.
   * @param key The key to insert.
   * @param value The corresponding value to insert.
   * @return The value associated with the inserted key or NULL if the key
   * exists.
   */
  const void *(*insert)(void *map, const void *key, const void *value);
  /**
   * @brief Insert a key-value pair into the HashMap or replace an existing
   * key's value.
   *
   * This function inserts a key-value pair into the HashMap or replaces the
   * value of an existing key.
   *
   * @param map A pointer to the HashMap.
   * @param key The key to insert or replace.
   * @param value The corresponding value to insert or replace.
   * @param replaced If not NULL and the key already exists, this flag is set
   * to 1.
   * @return The value associated with the inserted key or the new value if
   * replaced.
   */
  const void *(*insert_or_replace)(void *map, void *key, void *value,
                                   char *replaced);

  /**
   * @brief Replace a key's value using its hash value.
   *
   * This function replaces a key's value using its hash value. It is used when
   * the key and hash are known to correspond.
   *
   * @param map A pointer to the HashMap.
   * @param key The key to replace.
   * @param value The new value to set.
   * @param hash The hash value of the key.
   * @return The previous value associated with the key.
   */
  const void *(*replace_key_using_hash)(void *map, void *key, void *value,
                                        int hash);
  /**
   * @brief Get the number of key-value pairs in the HashMap.
   *
   * @param map A pointer to the HashMap.
   * @return The number of key-value pairs in the HashMap.
   */
  const int (*count)(void *map);

  /**
   * @brief Delete the HashMap and its associated resources.
   *
   * @param map A pointer to the HashMap to delete.
   */
  void (*del)(void *map);

  /**
   * @brief Initialize the iterator to the beginning of the HashMap.
   *
   * @param map A pointer to the HashMap.
   */
  void (*iter_begin)(void *map);

  /**
   * @brief Get the next key-value pair during iteration.
   *
   * @param map A pointer to the HashMap.
   * @return A pointer to the next key-value pair or NULL if the end of the
   * HashMap is reached.
   */
  void **(*iter_next)(void *map);
} LaudHashMapFn;

#endif