#ifndef ENGINE_COLLECTIONS_HASHMAP_H
#define ENGINE_COLLECTIONS_HASHMAP_H

#include <cc_hashtable.h>

#include "common.h"

typedef struct hashMap {
	CC_HashTable* cc_hashTable;
	size_t itemSize;
	ItemDestructorFn itemDestructor;
} HashMap;

typedef struct hashMapIter {
	CC_HashTableIter cc_hashTableIter;
	HashMap* hashMap;
} HashMapIter;

typedef struct keyValuePair {
	const char* key;
	void* value;
} KeyValuePair;

HashMap* HashMap_create(size_t itemSize, ItemDestructorFn itemDestructor);
void HashMap_destroy(HashMap* hashMap);

size_t HashMap_getSize(const HashMap* hashMap);

void HashMap_clear(HashMap* hashMap);

bool HashMap_hasKey(const HashMap* hashMap, const char* key);

void* HashMap_getItem(const HashMap* hashMap, const char* key);

void* HashMap_insertItem(HashMap* hashMap, const char* key, const void* item);
void HashMap_removeItem(HashMap* hashMap, const char* key, void* out);

HashMapIter HashMap_begin(HashMap* hashMap);

const KeyValuePair* HashMapIter_getNext(HashMapIter* it);

void HashMapIter_removeItem(HashMapIter* it, void* out);

#endif //ENGINE_COLLECTIONS_HASHMAP_H
