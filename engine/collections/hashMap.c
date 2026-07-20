#include "hashMap.h"

#include <stdlib.h>

#include "core/core.h"

static u64 hash(const char* str);
static int compareBucketEntryToKey(const void* bucketEntry, const void* key);

HashMap* HashMap_create(const size_t bucketCount, const size_t itemSize) {
	// Return a nullptr if the item size is zero
	if (itemSize == 0) {
		return nullptr;
	}

	// Allocate memory for the HashMap
	HashMap* hashMap = Memory_malloc(sizeof(*hashMap));
	if (!hashMap) {
		return nullptr;
	}

	// Initialize the HashMap
	hashMap->itemSize = itemSize;
	hashMap->bucketCount = bucketCount;

	// Allocate memory for the buckets
	hashMap->buckets = Memory_calloc(bucketCount, sizeof(LinkedList*));
	if (!hashMap->buckets) {
		Memory_free(hashMap);
		return nullptr;
	}

	return hashMap;
}

void HashMap_destroy(HashMap* hashMap) {

}

void* HashMap_insert(HashMap* hashMap, const char* key, const void* item) {
	// Return a nullptr if the HashMap is a nullptr
	if (!hashMap) {
		return nullptr;
	}

	// Get the bucket to insert the item to
	const size_t index = hash(key) % hashMap->bucketCount;

	// Initialize the bucket if it is a nullptr
	if (!hashMap->buckets[index]) {
		hashMap->buckets[index] = LinkedList_create(sizeof(BucketEntry));
		if (!hashMap->buckets[index]) {
			return nullptr;
		}
	}

	// Return a nullptr if an entry with this key already exists
	if (LinkedList_find(hashMap->buckets[index], key, compareBucketEntryToKey)) {
		return nullptr;
	}

	// Create the bucket entry
	BucketEntry entry;
	entry.key = key;
	entry.item = Memory_malloc(hashMap->itemSize);
	if (!entry.item) { return nullptr; }
	Memory_copy(entry.item, item, hashMap->itemSize);

	// Add the item to the bucket
	BucketEntry* bucketEntry = LinkedList_pushBack(hashMap->buckets[index], &entry);
	if (!bucketEntry) {
		return nullptr;
	}

	// Return the bucket entry's item
	return bucketEntry->item;
}

void HashMap_remove(HashMap* hashMap, const char* key) {
	// Return if the HashMap is a nullptr
	if (!hashMap) {
		return;
	}

	// Get the bucket to remove the item from
	const size_t index = hash(key) % hashMap->bucketCount;

	// Check if the bucket is a nullptr
	if (!hashMap->buckets[index]) {
		return;
	}

	// Search the bucket for a bucket entry with a matching key
	LinkedListIterator iterator = LinkedList_begin(hashMap->buckets[index]);
	while (LinkedListIterator_hasNext(&iterator)) {
		const BucketEntry* entry = LinkedListIterator_next(&iterator);
		if (strcmp(entry->key, key) == 0) {
			LinkedListIterator_remove(&iterator);
			return;
		}
	}
}

u64 hash(const char* str) {
	u64 hash = 5381;
	char c;
	while ((c = *str++)) {
		hash = (hash << 5) + hash + c;
	}
	return hash;
}

int compareBucketEntryToKey(const void* bucketEntry, const void* key) {
	const BucketEntry* e = bucketEntry;
	return strcmp((void*)e->key, key);
}
