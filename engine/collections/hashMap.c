#include "hashMap.h"

#include <assert.h>
#include <string.h>

#include "core/core.h"

static u64 hashKey(const char* key);
static size_t getBucketIndex(const HashMap* hashMap, const char* key);

static KeyValuePair* getKeyValuePairFromBucket(LinkedList* bucket, const char* key);

HashMap* HashMap_create(size_t bucketCount, size_t itemSize, CollectionsItemDestructorFn itemDestructor) {
    // Return null if the bucket count is zero
    if (bucketCount == 0) {
        Logger_error("Failed to create HashMap: Bucket count cannot be zero");
        return nullptr;
    }

    // Return null if the item size is zero
    if (itemSize == 0) {
        Logger_error("Failed to create HashMap: Item size cannot be zero");
        return nullptr;
    }

    // Allocate memory for the HashMap instance
    HashMap* hashMap = Memory_malloc(sizeof(*hashMap));
    if (!hashMap) {
        Logger_error("Failed to create HashMap: Memory allocation failed for HashMap instance");
        return nullptr;
    }

    // Initialize HashMap fields
    hashMap->itemSize = itemSize;
    hashMap->bucketCount = bucketCount;
    hashMap->itemDestructor = itemDestructor;

    // Allocate memory for HashMap buckets
    hashMap->buckets = Memory_calloc(bucketCount, sizeof(LinkedList*));
    if (!hashMap->buckets) {
        Logger_error("Failed to create HashMap: Memory allocation failed for HashMap buckets");
        Memory_free(hashMap);
        return nullptr;
    }

    return hashMap;
}

bool HashMap_hasKey(HashMap* hashMap, const char* key) {
    // Return null if the HashMap is null
    if (!hashMap) {
        Logger_error("Failed to check for key in HashMap: HashMap is null");
        return false;
    }

    // Return null if the key is null
    if (!key) {
        Logger_error("Failed to check for key in HashMap: Key is null");
        return false;
    }

    // Return false if the bucket does not exist
    size_t index = getBucketIndex(hashMap, key);
    LinkedList* bucket = hashMap->buckets[index];
    if (!bucket) {
        return false;
    }

    // Return whether a KeyValuePair exists with the specified key
    return (bool)getKeyValuePairFromBucket(bucket, key);
}

void* HashMap_getItem(HashMap* hashMap, const char* key) {
    // Return null if the HashMap is null
    if (!hashMap) {
        Logger_error("Failed to get item from HashMap: HashMap is null");
        return nullptr;
    }

    // Return null if the key is null
    if (!key) {
        Logger_error("Failed to get item from HashMap: Key is null");
        return nullptr;
    }

    // Try to get the item from the HashMap
    void* item;
    bool success = HashMap_tryGetItem(hashMap, key, &item);

    // Return null if the item could not be retrieved
    if (!success) {
        Logger_error("Failed to get item from HashMap: Item with key \"%s\" does not exist", key);
        return nullptr;
    }

    // Return the item
    return item;
}

bool HashMap_tryGetItem(HashMap* hashMap, const char* key, void** item) {
    // Set the item pointer to null
    *item = nullptr;

    // Return false if the HashMap is null
    if (!hashMap) {
        Logger_error("Failed to get item from HashMap: HashMap is null");
        return false;
    }

    // Return false if the key is null
    if (!key) {
        Logger_error("Failed to get item from HashMap: Key is null");
        return false;
    }

    // Return false if the key does not exist in the HashMap
    size_t index = getBucketIndex(hashMap, key);
    LinkedList* bucket = hashMap->buckets[index];
    KeyValuePair* keyValuePair;
    if (!bucket || !((keyValuePair = getKeyValuePairFromBucket(bucket, key)))) {
        return false;
    }

    // Set the item pointer and return success
    *item = keyValuePair->value;
    return true;
}

void* HashMap_insertItem(HashMap* hashMap, const char* key, const void* item) {
    // Return null if the HashMap is null
    if (!hashMap) {
        Logger_error("Failed to insert item to HashMap: HashMap is null");
        return nullptr;
    }

    // Return null if the key is null
    if (!key) {
        Logger_error("Failed to insert item to HashMap: Key is null");
        return nullptr;
    }

    // Return null if the item is null
    if (!item) {
        Logger_error("Failed to insert item to HashMap: Item is null");
        return nullptr;
    }

    // Create the bucket if it does not exist
    size_t index = getBucketIndex(hashMap, key);
    if (!hashMap->buckets[index]) {
        hashMap->buckets[index] = LinkedList_create(sizeof(KeyValuePair), nullptr);
        if (!hashMap->buckets[index]) {
            Logger_error("Failed to insert item to HashMap: Memory allocation failed for HashMap bucket");
            return nullptr;
        }
    }

    // Return null if the key already exists in the bucket
    LinkedList* bucket = hashMap->buckets[index];
    if (getKeyValuePairFromBucket(bucket, key)) {
        Logger_error("Failed to insert item to HashMap: Item with key \"%s\" already exists", key);
        return nullptr;
    }

    // Copy the key to a new memory location
    char* keyCopy = Memory_malloc((strlen(key) + 1) * sizeof(char));
    if (!keyCopy) {
        Logger_error("Failed to insert item to HashMap: Memory allocation failed for key");
        return nullptr;
    }
    strcpy(keyCopy, key);

    // Copy the item to a new memory location
    void* itemCopy = Memory_malloc(hashMap->itemSize);
    if (!itemCopy) {
        Logger_error("Failed to insert item to HashMap: Memory allocation failed for key");
        Memory_free(keyCopy);
        return nullptr;
    }
    memcpy(itemCopy, item, hashMap->itemSize);

    // Insert the key and item into the bucket as a KeyValuePair
    KeyValuePair keyValuePair = {
        .key = keyCopy,
        .value = itemCopy,
    };
    KeyValuePair* result = LinkedList_pushBackItem(bucket, &keyValuePair);

    // Return null if the KeyValuePair could not be inserted
    if (!result) {
        Logger_error("Failed to insert item to HashMap: Failed to push back KeyValuePair to bucket");
        Memory_free(keyCopy);
        Memory_free(itemCopy);
        return nullptr;
    }

    // Sort the bucket by key
    LinkedList_sort(bucket, (CollectionsCompareFn)strcmp);

    // Log a warning if the length of the bucket has exceeded 5
    if (LinkedList_getLength(bucket) > HASHMAP_BUCKET_SIZE_WARNING_THRESHOLD) {
        Logger_warning(
            "HashMap bucket has exceeded %d items. "
            "Consider increasing the HashMap's bucket count for efficiency's sake",
            HASHMAP_BUCKET_SIZE_WARNING_THRESHOLD);
    }

    // Return the result
    return result;
}

bool HashMap_forEach(HashMap* hashMap, CollectionsForEachActionFn action) {
    // Return failure if the HashMap is null
    if (!hashMap) {
        Logger_error("Failed to execute forEach on HashMap: HashMap is null");
        return false;
    }

    // Return failure if the action is null
    if (!action) {
        Logger_error("Failed to execute forEach on HashMap: Action is null");
        return false;
    }

    // Iterate through each bucket in the HashMap
    for (size_t i = 0; i < hashMap->bucketCount; i++) {
        // Skip any null buckets
        LinkedList* bucket = hashMap->buckets[i];
        if (!bucket) {
            continue;
        }

        // Execute action for each KeyValuePair in the bucket
        LinkedList_forEach(bucket, action);
    }

    // Return success
    return true;
}

u64 hashKey(const char* key) {
    assert(key);

    // Define constants
    constexpr u64 FNV_SEED = 0xcbf29ce484222325;
    constexpr u64 FNV_PRIME = 0x100000001b3;

    // Hash the key
    const u8* p = (u8*)key;
    u64 hash = FNV_SEED;
    u8 c;
    while ((c = *p++)) {
        hash = (hash ^ c) * FNV_PRIME;
    }

    // Return the result
    return hash;
}

size_t getBucketIndex(const HashMap* hashMap, const char *key) {
    assert(hashMap);
    assert(key);

    const u64 hash = hashKey(key);
    return hash % hashMap->bucketCount;
}

KeyValuePair* getKeyValuePairFromBucket(LinkedList* bucket, const char* key) {
    assert(bucket);
    assert(key);

    // Iterate through the bucket until a match is found
    LinkedListIterator it = LinkedList_begin(bucket);
    KeyValuePair* keyValuePair;
    while ((keyValuePair = LinkedListIterator_next(&it))) {
        if (strcmp(keyValuePair->key, key) == 0) {
            return keyValuePair;
        }
    }

    return nullptr;
}