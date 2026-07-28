#include "hashMap.h"

#include "engine/logger.h"

static void destroyKeyValuePair(const HashMap* hashMap, KeyValuePair* keyValuePair);

HashMap* HashMap_create(const size_t itemSize, const ItemDestructorFn itemDestructor) {
	// Return null if the item size is 0
	COLLECTIONS_REQUIRE(itemSize != 0, "Failed to create HashMap: Item size cannot be 0", nullptr);

	// Create the HashMap
	HashMap* hashMap = malloc(sizeof(*hashMap));
	COLLECTIONS_REQUIRE(hashMap, "Failed to create HashMap: Memory allocation failed", nullptr);
	hashMap->itemSize = itemSize;
	hashMap->itemDestructor = itemDestructor;

	// Create the CC_HashTable config
	CC_HashTableConf hashTableConfig;
	cc_hashtable_conf_init(&hashTableConfig);
	hashTableConfig.mem_alloc = malloc;
	hashTableConfig.mem_calloc = calloc;
	hashTableConfig.mem_free = free;

	// Create the HashMap's CC_HashTable
	const CC_Stat status = cc_hashtable_new_conf(&hashTableConfig, &hashMap->cc_hashTable);
	COLLECTIONS_REQUIRE(status == CC_OK, "Failed to create HashMap: Memory allocation failed", nullptr);

	// Return the newly created HashMap
	return hashMap;
}

void HashMap_destroy(HashMap* hashMap) {
	// Return if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to destroy HashMap: HashMap is null");
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to destroy HashMap: HashMap state is invalid");

	// Call the destructor on each KeyValuePair
	char* key;
	KeyValuePair* keyValuePair;
	CC_HASHTABLE_FOREACH(hashMap->cc_hashTable, key, keyValuePair, {
		destroyKeyValuePair(hashMap, keyValuePair);
	});

	// Destroy the HashMap
	cc_hashtable_destroy(hashMap->cc_hashTable);
	free(hashMap);
}

size_t HashMap_getSize(const HashMap* hashMap) {
	// Return SIZE_MAX if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to get size of HashMap: HashMap is null", SIZE_MAX);
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to get size of HashMap: HashMap state is invalid", SIZE_MAX);

	// Get the size of
	return cc_hashtable_size(hashMap->cc_hashTable);
}

void HashMap_clear(HashMap* hashMap) {
	// Return if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to clear HashMap: HashMap is null");
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to clear HashMap: HashMap state is invalid");

	// Call the destructor on each KeyValuePair
	char* key;
	KeyValuePair* keyValuePair;
	CC_HASHTABLE_FOREACH(hashMap->cc_hashTable, key, keyValuePair, {
		destroyKeyValuePair(hashMap, keyValuePair);
	});

	// Remove all items from the HashMap
	cc_hashtable_remove_all(hashMap->cc_hashTable);
}

bool HashMap_hasKey(const HashMap* hashMap, const char* key) {
	// Return false if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to check HashMap for key: HashMap is null", false);
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to check HashMap for key: HashMap state is invalid", false);

	// Return false if the key is null
	COLLECTIONS_REQUIRE(key, "Failed to check HashMap for key: Key is null", false);

	// Get whether the HashMap contains the key
	return cc_hashtable_contains_key(hashMap->cc_hashTable, (void*)key);
}

KeyValuePair* HashMap_getKeyValuePair(const HashMap* hashMap, const char* key) {
	// Return null if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to get KeyValuePair from HashMap: HashMap is null", nullptr);
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to get KeyValuePair from HashMap: HashMap state is invalid", nullptr);

	// Return null if the key is null
	COLLECTIONS_REQUIRE(key, "Failed to get KeyValuePair from HashMap: Key is null", nullptr);

	// Get the KeyValuePair from the HashMap
	KeyValuePair* keyValuePair;
	const CC_Stat status = cc_hashtable_get(hashMap->cc_hashTable, (void*)key, (void**)&keyValuePair);
	if (status != CC_OK) {
		return nullptr;
	}

	// Return the KeyValuePair's value
	return keyValuePair;
}

void* HashMap_getItem(const HashMap* hashMap, const char* key) {
	return HashMap_getKeyValuePair(hashMap, key)->value;
}

KeyValuePair* HashMap_insertItem(HashMap* hashMap, const char* key, const void* item) {
	// Return null if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to insert item to HashMap: HashMap is null", nullptr);
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to insert item to HashMap: HashMap state is invalid", nullptr);

	// Return null if the key or item is null
	COLLECTIONS_REQUIRE(key, "Failed to insert item to HashMap: Key is null", nullptr);
	COLLECTIONS_REQUIRE(item, "Failed to insert item to HashMap: Item is null", nullptr);

	// Return null if the key already exists in the HashMap
	if (HashMap_hasKey(hashMap, key)) {
		Logger_error("Failed to insert item to HashMap: Key \"%s\" already exists in the HashMap", key);
		return nullptr;
	}

	// Allocate memory for the KeyValuePair, key copy, and item copy
	const size_t keyLen = strlen(key) + 1;
	KeyValuePair* keyValuePair = malloc(sizeof(*keyValuePair));
	char* keyCopy = malloc(keyLen);
	void* itemCopy = malloc(hashMap->itemSize);
	if (!keyValuePair || !keyCopy || !itemCopy) {
		Logger_error("Failed to insert item to HashMap: Memory allocation failed");
		if (keyValuePair) { free(keyValuePair); }
		if (keyCopy) { free(keyCopy); }
		if (itemCopy) { free(itemCopy); }
		return nullptr;
	}

	// Initialize KeyValuePair
	strcpy(keyCopy, key);
	memcpy(itemCopy, item, hashMap->itemSize);
	keyValuePair->key = keyCopy;
	keyValuePair->value = itemCopy;

	// Add the key and item to the HashMap
	const CC_Stat status = cc_hashtable_add(hashMap->cc_hashTable, keyCopy, keyValuePair);
	if (status != CC_OK) {
		Logger_error("Failed to insert item to HashMap: Memory allocation failed");
		free(keyValuePair);
		free(keyCopy);
		free(itemCopy);
		return nullptr;
	}

	// Return the newly inserted KeyValuePair
	return keyValuePair;
}

void HashMap_removeItem(HashMap* hashMap, const char* key, void* out) {
	// Return if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to remove item from HashMap: HashMap is null");
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to remove item from HashMap: HashMap state is invalid");

	// Return if the key is null
	COLLECTIONS_REQUIRE(key, "Failed to remove item from HashMap: Key is null");

	// Remove the KeyValuePair from the HashMap
	KeyValuePair* keyValuePair;
	const CC_Stat status = cc_hashtable_remove(hashMap->cc_hashTable, (void*)key, (void**)&keyValuePair);
	if (status != CC_OK) {
		Logger_error("Failed to remove item from HashMap: Key \"%s\" does not exist in the HashMap", key);
		return;
	}

	// Copy the KeyValuePair's value to out if out is not null
	if (out) { memcpy(out, keyValuePair->value, hashMap->itemSize); }

	// Destroy the KeyValuePair
	destroyKeyValuePair(hashMap, keyValuePair);
}

HashMapIter HashMap_begin(HashMap* hashMap) {
	// Return if the HashMap is null or invalid
	COLLECTIONS_REQUIRE(hashMap, "Failed to create HashMapIter: HashMap is null", (HashMapIter){});
	COLLECTIONS_REQUIRE(hashMap->cc_hashTable, "Failed to create HashMapIter: HashMap state is invalid", (HashMapIter){});

	// Initialize the iterator
	HashMapIter it;
	cc_hashtable_iter_init(&it.cc_hashTableIter, hashMap->cc_hashTable);
	it.hashMap = hashMap;

	// Return the iterator
	return it;
}

const KeyValuePair* HashMapIter_getNext(HashMapIter* it) {
	// Return false if the iterator is null
	COLLECTIONS_REQUIRE(it, "Failed to advance HashMapIter: HashMapIter is null", nullptr);
	COLLECTIONS_REQUIRE(it->cc_hashTableIter.table, "Failed to advance HashMapIter: HashMapIter is invalid", nullptr);

	// Advance the iterator
	TableEntry* tableEntry;
	const CC_Stat status = cc_hashtable_iter_next(&it->cc_hashTableIter, &tableEntry);
	if (status != CC_OK) {
		return nullptr;
	}

	// Return the KeyValuePair
	return tableEntry->value;
}

void HashMapIter_removeItem(HashMapIter* it, void* out) {
	// Return if the iterator is null
	COLLECTIONS_REQUIRE(it, "Failed to remove item with HashMapIter: HashMapIter is null");
	COLLECTIONS_REQUIRE(it->cc_hashTableIter.table, "Failed to remove item with HashMapIter: HashMapIter is invalid");

	// Remove the KeyValuePair from the HashMap
	KeyValuePair* keyValuePair;
	const CC_Stat status = cc_hashtable_iter_remove(&it->cc_hashTableIter, (void**)&keyValuePair);
	if (status != CC_OK) {
		Logger_error("Failed to remove item with HashMapIter: No item to remove");
		return;
	}

	// Copy the KeyValuePair's value to out if out is not null
	if (out) { memcpy(out, keyValuePair->value, it->hashMap->itemSize); }

	// Destroy the KeyValuePair
	destroyKeyValuePair(it->hashMap, keyValuePair);
}

void destroyKeyValuePair(const HashMap* hashMap, KeyValuePair* keyValuePair) {
	if (hashMap->itemDestructor) {
		hashMap->itemDestructor(keyValuePair->value);
	}

	free(keyValuePair->value);
	free((char*)keyValuePair->key);
	free(keyValuePair);
}