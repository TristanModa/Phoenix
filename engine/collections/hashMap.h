#ifndef ENGINE_COLLECTIONS_HASHMAP_H
#define ENGINE_COLLECTIONS_HASHMAP_H

#include <cc_hashtable.h>

#include "common.h"

/**
 * An unordered Key-Value map.
 * Wrapper for CC_HashTable.
 */
typedef struct hashMap {
	/**
	 * The underlying CC_HashTable
	 */
	CC_HashTable* cc_hashTable;
	/**
	 * The size of an item in bytes
	 */
	size_t itemSize;
	/**
	 * The destructor to call on item destruction
	 */
	ItemDestructorFn itemDestructor;
} HashMap;

/**
 * Iterator for a HashMap.
 * Wrapper for CC_HashTableIter.
 */
typedef struct hashMapIter {
	/**
	 * The underlying CC_hashTableIter
	 */
	CC_HashTableIter cc_hashTableIter;
	/**
	 * The HashMap of this iterator
	 */
	HashMap* hashMap;
} HashMapIter;

/**
 * Data entry stored within a HashMap
 */
typedef struct keyValuePair {
	const char* key;
	void* value;
} KeyValuePair;

/**
 * Allocates and initializes an instance of a HashMap
 * @param itemSize The size of an item in the HashMap
 * @param itemDestructor The destructor for a HashMap item. Pass null for no destructor.
 * @return The newly created HashMap, or null on failure.
 * The caller owns the returned pointer and is responsible for freeing it using HashMap_destroy()
 */
HashMap* HashMap_create(size_t itemSize, ItemDestructorFn itemDestructor);
/**
 * Destroys a Hash Map and releases all associated memory
 * @param hashMap The HashMap to destroy
 */
void HashMap_destroy(HashMap* hashMap);

/**
 * Gets the number of items in a HashMap
 * @param hashMap The HashMap to get the size of
 * @return The size of the HashMap, or SIZE_MAX on failure
 */
size_t HashMap_getSize(const HashMap* hashMap);

/**
 * Removes and destroys all items from a ArrayList
 * @param hashMap The ArrayList to clear
 */
void HashMap_clear(HashMap* hashMap);

/**
 * Gets whether a HashMap contains a key
 * @param hashMap The HashMap to check
 * @param key The key to check for
 * @return True if it does
 */
bool HashMap_hasKey(const HashMap* hashMap, const char* key);

/**
 * Gets the KeyValuePair with the specified key from a HashMap
 * @param hashMap The HashMap to get from
 * @param key The key of the KeyValuePair to get
 * @return The KeyValuePair, null if the key does not exist in the HashMap, or null on failure
 */
KeyValuePair* HashMap_getKeyValuePair(const HashMap* hashMap, const char* key);
/**
 * Gets the item with the specified key from a HashMap
 * @param hashMap The HashMap to get from
 * @param key The key associated with the item to get
 * @return The item, null if the key does not exist in the HashMap, or null on failure
 */
void* HashMap_getItem(const HashMap* hashMap, const char* key);

/**
 *
 * @param hashMap
 * @param key
 * @param item
 * @return
 */
KeyValuePair* HashMap_insertItem(HashMap* hashMap, const char* key, const void* item);
void HashMap_removeItem(HashMap* hashMap, const char* key, void* out);

HashMapIter HashMap_begin(HashMap* hashMap);

const KeyValuePair* HashMapIter_getNext(HashMapIter* it);

void HashMapIter_removeItem(HashMapIter* it, void* out);

#endif //ENGINE_COLLECTIONS_HASHMAP_H
