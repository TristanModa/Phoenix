#ifndef ENGINE_COLLECTIONS_HASHMAP_H
#define ENGINE_COLLECTIONS_HASHMAP_H

#include "core/core.h"
#include "linkedList.h"

typedef struct bucketEntry {
	const char* key;
	void* item;
} BucketEntry;

typedef struct hashMap {
	size_t itemSize;
	size_t bucketCount;
	LinkedList** buckets;
} HashMap;

HashMap* HashMap_create(size_t bucketCount, size_t itemSize);
void HashMap_destroy(HashMap* hashMap);

void* HashMap_insert(HashMap* hashMap, const char* key, const void* item);
void HashMap_remove(HashMap* hashMap, const char* key);

#endif //ENGINE_COLLECTIONS_HASHMAP_H
