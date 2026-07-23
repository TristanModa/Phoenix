#ifndef ENGINE_COLLECTIONS_HASHMAP_H
#define ENGINE_COLLECTIONS_HASHMAP_H

#include <stddef.h>

#include "common.h"
#include "linkedList.h"

#define HASHMAP_BUCKET_SIZE_WARNING_THRESHOLD (4)

typedef struct keyValuePair {
    const char* key;
    void* value;
} KeyValuePair;

typedef struct hashMap {
    size_t itemSize;
    size_t bucketCount;
    LinkedList** buckets;
    CollectionsItemDestructorFn itemDestructor;
} HashMap;

HashMap* HashMap_create(size_t bucketCount, size_t itemSize, CollectionsItemDestructorFn itemDestructor);

bool HashMap_hasKey(HashMap* hashMap, const char* key);

void* HashMap_getItem(HashMap* hashMap, const char* key);
bool HashMap_tryGetItem(HashMap* hashMap, const char* key, void** item);

void* HashMap_insertItem(HashMap* hashMap, const char* key, const void* item);
bool HashMap_removeItem(HashMap* hashMap, const char* key, void* out);

bool HashMap_forEach(HashMap* hashMap, CollectionsForEachActionFn action);

#endif //ENGINE_COLLECTIONS_HASHMAP_H