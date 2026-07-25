#include "arrayList.h"

#include "core/core.h"

static void destroyItem(const ArrayList* arrayList, void* item);

ArrayList* ArrayList_create(const size_t itemSize, const ItemDestructorFn itemDestructor) {
	// Return null if the item size is 0
	COLLECTIONS_REQUIRE(itemSize != 0, "Failed to create ArrayList: Item size cannot be 0", nullptr);

	// Create the ArrayList
	ArrayList* arrayList = Memory_malloc(sizeof(*arrayList));
	COLLECTIONS_REQUIRE(arrayList, "Failed to create ArrayList: Memory allocation failed", nullptr);
	arrayList->itemSize = itemSize;
	arrayList->itemDestructor = itemDestructor;

	// Create the CC_Array config
	const CC_ArrayConf arrayConfig = {
		.capacity = 8,
		.exp_factor = 2,
		.mem_alloc = Memory_malloc,
		.mem_calloc = Memory_calloc,
		.mem_free = Memory_free
	};

	// Create the ArrayList's CC_Array
	const CC_Stat status = cc_array_new_conf(&arrayConfig, &arrayList->cc_array);
	if (status != CC_OK) {
		if (status == CC_ERR_ALLOC) {
			Logger_error("Failed to create ArrayList: Memory allocation failed");
		} else {
			Logger_error("Failed to create ArrayList: Invalid capacity");
		}
		Memory_free(arrayList);
		return nullptr;
	}

	// Return the newly created ArrayList
	return arrayList;
}

void ArrayList_destroy(ArrayList* arrayList) {
	// Return if the ArrayList is null
	COLLECTIONS_REQUIRE(arrayList, "Failed to destroy ArrayList: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to destroy ArrayList: ArrayList state is invalid");

	// Call the destructor on each item
	CC_ARRAY_FOREACH(item, arrayList->cc_array, {
		destroyItem(arrayList, item);
	});

	// Destroy the ArrayList
	cc_array_destroy(arrayList->cc_array);
	Memory_free(arrayList);
}

size_t ArrayList_getSize(const ArrayList* arrayList) {
	// Return SIZE_MAX if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to get size of ArrayList: ArrayList is null", SIZE_MAX);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to get size of ArrayList: ArrayList state is invalid", SIZE_MAX);

	// Return the length of the ArrayList
	return cc_array_size(arrayList->cc_array);
}

void ArrayList_clear(ArrayList* arrayList) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to clear ArrayList: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to clear ArrayList: ArrayList state is invalid");

	// Call the destructor on each item
	CC_ARRAY_FOREACH(item, arrayList->cc_array, {
		destroyItem(arrayList, item);
	});

	// Remove all items from the ArrayList
	cc_array_remove_all(arrayList->cc_array);
}

void ArrayList_trimCapacity(ArrayList* arrayList) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to trim capacity of ArrayList: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to trim capacity of ArrayList: ArrayList state is invalid");

	// Shrink the ArrayList
	const CC_Stat status = cc_array_trim_capacity(arrayList->cc_array);
	if (status != CC_OK) {
		Logger_error("Failed to trim capacity of ArrayList: Memory allocation failed");
	}
}

void* ArrayList_getItem(const ArrayList* arrayList, const size_t index) {
	// Return null if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to get item from ArrayList: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to get item from ArrayList: ArrayList state is invalid", nullptr);

	// Get the item from the ArrayList
	void* item;
	const CC_Stat status = cc_array_get_at(arrayList->cc_array, index, &item);
	if (status != CC_OK) {
		Logger_error(
			"Failed to get item from ArrayList: Index %zu out of bounds for ArrayList of length %zu",
			index, ArrayList_getSize(arrayList));
		return nullptr;
	}

	// Return the item
	return item;
}

void* ArrayList_getBackItem(const ArrayList* arrayList) {
	// Return null if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to get back item of ArrayList: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to get back item of ArrayList: ArrayList state is invalid", nullptr);

	// Get the size of the ArrayList
	const size_t size = ArrayList_getSize(arrayList);

	// Return if the ArrayList is empty
	COLLECTIONS_REQUIRE(size != 0, "Failed to get back item of ArrayList: ArrayList is empty", nullptr);

	// Get the item at the end of the ArrayList
	return ArrayList_getItem(arrayList, size - 1);
}

void* ArrayList_insertItem(ArrayList* arrayList, const size_t index, const void* item) {
	// Return null if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to insert item to ArrayList: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to insert item to ArrayList: ArrayList state is invalid", nullptr);

	// Return null if the item is null
	COLLECTIONS_REQUIRE(item, "Failed to insert item to ArrayList: Item is null", nullptr);

	// Allocate memory for the item
	void* itemCopy = Memory_malloc(arrayList->itemSize);
	COLLECTIONS_REQUIRE(itemCopy, "Failed to insert item to ArrayList: Memory allocation failed", nullptr);
	memcpy(itemCopy, item, arrayList->itemSize);

	// Add the item to the ArrayList
	const CC_Stat status = cc_array_add_at(arrayList->cc_array, itemCopy, index);
	if (status != CC_OK) {
		if (status == CC_ERR_ALLOC) {
			Logger_error("Failed to insert item to ArrayList: Memory allocation failed");
		} else if (status == CC_ERR_MAX_CAPACITY) {
			Logger_error("Failed to insert item to ArrayList: Max capacity reached");
		} else {
			Logger_error(
				"Failed to insert item to ArrayList: Index %zu out of range for ArrayList of length %zu",
				index, ArrayList_getSize(arrayList));
		}
		Memory_free(itemCopy);
		return nullptr;
	}

	// Return the newly added item
	return itemCopy;
}

void ArrayList_removeItem(ArrayList* arrayList, const size_t index, void* out) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to remove item from ArrayList: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to remove item from ArrayList: ArrayList state is invalid");

	// Remove the item from the ArrayList
	void* item;
	const CC_Stat status = cc_array_remove_at(arrayList->cc_array, index, &item);
	if (status != CC_OK) {
		Logger_error(
			"Failed to remove item from ArrayList: Index %zu out of range for ArrayList of length %zu",
			index, ArrayList_getSize(arrayList));
		return;
	}

	// Copy the item to out if out is not null
	if (out) { memcpy(out, item, arrayList->itemSize); }

	// Destroy the item
	destroyItem(arrayList, item);
}

void* ArrayList_replaceItem(ArrayList* arrayList, const size_t index, const void* item, void* out) {
	// Return null if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to replace item in ArrayList: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to replace item in ArrayList: ArrayList state is invalid", nullptr);

	// Return null if the item is null
	COLLECTIONS_REQUIRE(item, "Failed to replace item in ArrayList: Replacement item is null", nullptr);

	// Allocate memory for the new item
	void* newItem = Memory_malloc(arrayList->itemSize);
	COLLECTIONS_REQUIRE(newItem, "Failed to replace item in ArrayList: Memory allocation failed", nullptr);
	memcpy(newItem, item, arrayList->itemSize);

	// Replace the old item with the new item
	void* oldItem;
	const CC_Stat status = cc_array_replace_at(arrayList->cc_array, newItem, index, &oldItem);
	if (status != CC_OK) {
		Logger_error(
			"Failed to replace item in ArrayList: Index %zu out of range for ArrayList of length %zu",
			index, ArrayList_getSize(arrayList));
		Memory_free(newItem);
		return nullptr;
	}

	// Copy the old item to out, or destroy it in place if out is null
	if (out) { memcpy(out, oldItem, arrayList->itemSize); }

	// Destroy the old item
	destroyItem(arrayList, oldItem);

	// Return the newly inserted item
	return newItem;
}

void* ArrayList_pushBackItem(ArrayList* arrayList, const void* item) {
	// Return null if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to push back item to ArrayList: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to push back item to ArrayList: ArrayList state is invalid", nullptr);

	// Insert the item at the end of the ArrayList
	const size_t size = ArrayList_getSize(arrayList);
	return ArrayList_insertItem(arrayList, size, item);
}

void ArrayList_popBackItem(ArrayList* arrayList, void* out) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to pop back item from ArrayList: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to pop back item from ArrayList: ArrayList state is invalid");

	// Get the size of the ArrayList
	const size_t size = ArrayList_getSize(arrayList);

	// Return if the ArrayList is empty
	COLLECTIONS_REQUIRE(size != 0, "Failed to pop back item from ArrayList: ArrayList is empty");

	// Remove the item at the end of the ArrayList
	ArrayList_removeItem(arrayList, size - 1, out);
}

void ArrayList_forEach(ArrayList* arrayList, const ItemActionFn action) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to execute action on ArrayList items: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to execute action on ArrayList items: ArrayList state is invalid");

	// Return if the action function is null
	COLLECTIONS_REQUIRE(action, "Failed to execute action on ArrayList items: Action function is null");

	// Execute the action on each item of the ArrayList
	cc_array_map(arrayList->cc_array, action);
}

void* ArrayList_find(const ArrayList* arrayList, const void* key, const ItemCompareFn compare) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to search ArrayList for item: ArrayList is null", nullptr);
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to search ArrayList for item: ArrayList state is invalid", nullptr);

	// Return null if the key is null
	COLLECTIONS_REQUIRE(key, "Failed to search ArrayList for item: Key is null", nullptr);

	// Return null if the compare function is null
	COLLECTIONS_REQUIRE(compare, "Failed to search ArrayList for item: Compare function is null", nullptr);

	// Iterate through the ArrayList until a match is found
	void* result = nullptr;
	CC_ARRAY_FOREACH(item, arrayList->cc_array, {
		if (compare(item, key) == 0) {
			result = item;
			break;
		}
	});

	// Return result
	return result;
}

void ArrayList_sort(ArrayList* arrayList, const ItemCompareFn compare) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to sort ArrayList items: ArrayList is null");
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to sort ArrayList items: ArrayList state is invalid");

	// Return if the compare function is null
	COLLECTIONS_REQUIRE(compare, "Failed to sort ArrayList items: Compare function is null");

	// Sort the ArrayList
	cc_array_sort(arrayList->cc_array, compare);
}

ArrayListIter ArrayList_begin(ArrayList* arrayList) {
	// Return if the ArrayList is null or invalid
	COLLECTIONS_REQUIRE(arrayList, "Failed to create ArrayListIter: ArrayList is null", (ArrayListIter){});
	COLLECTIONS_REQUIRE(arrayList->cc_array, "Failed to create ArrayListIter: ArrayList state is invalid", (ArrayListIter){});

	// Initialize the iterator
	ArrayListIter it;
	cc_array_iter_init(&it.cc_arrayIter, arrayList->cc_array);
	it.arrayList = arrayList;

	// Return the iterator
	return it;
}

void* ArrayListIter_getNext(ArrayListIter* it) {
	// Return null if the iterator is null or invalid
	COLLECTIONS_REQUIRE(it, "Failed to advance ArrayListIter: ArrayListIter is null", nullptr);
	COLLECTIONS_REQUIRE(it->cc_arrayIter.ar, "Failed to advance ArrayListIter: ArrayListIter is invalid", nullptr);

	// Get the next item
	void* next;
	const CC_Stat status = cc_array_iter_next(&it->cc_arrayIter, &next);
	if (status != CC_OK) {
		return nullptr;
	}

	// Return the next item
	return next;
}

size_t ArrayListIter_getIndex(ArrayListIter* it) {
	// Return SIZE_MAX if the iterator is null or invalid
	COLLECTIONS_REQUIRE(it, "Failed to get index of ArrayListIter: ArrayListIter is null", SIZE_MAX);
	COLLECTIONS_REQUIRE(it->cc_arrayIter.ar, "Failed to get index of ArrayListIter: ArrayListIter is invalid", SIZE_MAX);

	// Return SIZE_MAX if ArrayListIter_next has not been called on this iterator
	COLLECTIONS_REQUIRE(
		it->cc_arrayIter.index != 0,
		"Failed to get index of ArrayListIter: Cannot get index of ArrayListIter that has not been advanced",
		SIZE_MAX);

	// Return the current index
	return cc_array_iter_index(&it->cc_arrayIter);
}

void* ArrayListIter_insertItem(ArrayListIter* it, const void* item) {
	// Return null if the iterator is null
	COLLECTIONS_REQUIRE(it, "Failed to insert item with ArrayListIter: ArrayListIter is null", nullptr);
	COLLECTIONS_REQUIRE(
		it->cc_arrayIter.ar,
		"Failed to insert item with ArrayListIter: ArrayListIter is invalid",
		nullptr);

	// Return null if the item is null
	COLLECTIONS_REQUIRE(item, "Failed to insert item with ArrayListIter: Item is null", nullptr);

	// Allocate memory for the item
	void* itemCopy = Memory_malloc(it->arrayList->itemSize);
	COLLECTIONS_REQUIRE(itemCopy, "Failed to insert item with ArrayListIter: Memory allocation failed", nullptr);
	memcpy(itemCopy, item, it->arrayList->itemSize);

	// Add the item to the ArrayList
	const CC_Stat status = cc_array_iter_add(&it->cc_arrayIter, itemCopy);
	if (status != CC_OK) {
		if (status == CC_ERR_ALLOC) {
			Logger_error("Failed to insert item with ArrayListIter: Memory allocation failed");
		} else {
			Logger_error("Failed to insert item with ArrayListIter: Max capacity reached");
		}
		Memory_free(itemCopy);
		return nullptr;
	}

	// Return the item
	return itemCopy;
}

void ArrayListIter_removeItem(ArrayListIter* it, void* out) {
	// Return if the iterator is null
	COLLECTIONS_REQUIRE(it, "Failed to remove item with ArrayListIter: ArrayListIter is null");
	COLLECTIONS_REQUIRE(it->cc_arrayIter.ar, "Failed to remove item with ArrayListIter: ArrayListIter is invalid");

	// Remove the item from the ArrayList
	void* item;
	const CC_Stat status = cc_array_iter_remove(&it->cc_arrayIter, &item);
	COLLECTIONS_REQUIRE(status == CC_OK, "Failed to remove item with ArrayListIter: No item to remove");

	// Copy the memory to out
	if (out) { memcpy(out, item, it->arrayList->itemSize); }

	// Destroy the item
	destroyItem(it->arrayList, item);
}

void* ArrayListIter_replaceItem(ArrayListIter* it, const void* item, void* out) {
	// Return null if the iterator is null
	COLLECTIONS_REQUIRE(it, "Failed to replace item with ArrayListIter: ArrayListIter is null", nullptr);
	COLLECTIONS_REQUIRE(
		it->cc_arrayIter.ar,
		"Failed to replace item with ArrayListIter: ArrayListIter is invalid",
		nullptr);

	// Return null if the item is null
	COLLECTIONS_REQUIRE(item, "Failed to replace item with ArrayListIter: Replacement item is null", nullptr);

	// Allocate memory for the new item
	void* newItem = Memory_malloc(it->arrayList->itemSize);
	COLLECTIONS_REQUIRE(newItem, "Failed to replace item with ArrayListIter: Memory allocation failed", nullptr);
	memcpy(newItem, item, it->arrayList->itemSize);

	// Replace the old item with the new item
	void* oldItem;
	const CC_Stat status = cc_array_iter_replace(&it->cc_arrayIter, newItem, &oldItem);
	if (status != CC_OK) {
		Logger_error("Failed to replace item with ArrayListIter: No item to replace");
		Memory_free(newItem);
		return nullptr;
	}

	// Copy the old item to out, or destroy it in place if out is null
	if (out) { memcpy(out, oldItem, it->arrayList->itemSize); }

	// Destroy the old item
	destroyItem(it->arrayList, oldItem);

	// Return the newly inserted item
	return newItem;
}

void destroyItem(const ArrayList* arrayList, void* item) {
	// Call the destructor on the item if it is not null
	if (arrayList->itemDestructor) {
		arrayList->itemDestructor(item);
	}

	// Free the item memory
	Memory_free(item);
}