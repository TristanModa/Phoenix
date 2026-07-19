#include "arrayList.h"

#include <stdlib.h>

#include "core/core.h"

ArrayList* ArrayList_create(const size_t itemSize, const size_t capacity) {
	// Throw a fatal error if the item size is zero
	COLLECTIONS_FATAL_IF(itemSize == 0,
		COLLECTIONS_ERR_FAILED_CREATE COLLECTIONS_ERR_ITEM_SIZE_ZERO,
		COLLECTIONS_ERR_ARRAY_LIST);

	// Allocate memory for the ArrayList struct
	ArrayList* arrayList = Memory_malloc(sizeof(*arrayList));
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_CREATE COLLECTIONS_ERR_MALLOC_FAIL,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST " instance");

	// Initialize ArrayList fields
	arrayList->itemSize = itemSize;
	arrayList->capacity = capacity;
	arrayList->length = 0;

	// Allocate memory for the ArrayList items
	arrayList->items = capacity != 0 ? Memory_malloc(itemSize * capacity) : nullptr;
	COLLECTIONS_FATAL_IF(!arrayList->items && capacity != 0,
		COLLECTIONS_ERR_FAILED_CREATE COLLECTIONS_ERR_MALLOC_FAIL,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST " items");

	return arrayList;
}

void ArrayList_destroy(ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_DESTROY COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Release the ArrayList and its items
	Memory_free(arrayList->items);
	Memory_free(arrayList);
}

size_t ArrayList_getLength(const ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_GET_LENGTH COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Return the length of the ArrayList
	return arrayList->length;
}

bool ArrayList_inBounds(const ArrayList* arrayList, const size_t index) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_BOUNDS_CHECK COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Return true if the index is less than the length
	return index < arrayList->length;
}

void ArrayList_clear(ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_CLEAR COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Set the length of the ArrayList to zero
	arrayList->length = 0;
}

void ArrayList_resize(ArrayList* arrayList, const size_t capacity) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		"Failed to resize ArrayList: " COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST);

	// Return if the new capacity is less than the length
	if (capacity < arrayList->length) {
		Logger_error(
			"Failed to resize ArrayList: " "New capacity (%zu) is not large enough to contain %zu items.",
			capacity, arrayList->length);
		return;
	}

	// Free the buffer if the desired capacity is zero
	if (capacity == 0) {
		Memory_free(arrayList->items);
		arrayList->items = nullptr;
		arrayList->capacity = 0;
		return;
	}

	// Reallocate the ArrayList items
	void* items = Memory_realloc(arrayList->items, arrayList->itemSize * capacity);
	COLLECTIONS_FATAL_IF(!items,
		"Failed to resize ArrayList" COLLECTIONS_ERR_MALLOC_FAIL,
		COLLECTIONS_ERR_ARRAY_LIST " items");

	// Update the ArrayList
	arrayList->capacity = capacity;
	arrayList->items = items;
}

void ArrayList_insert(ArrayList* arrayList, const void* item, const size_t index) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the item is a nullptr
	COLLECTIONS_FATAL_IF(!item,
		COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Item");

	// Use push back function if the item should be added to the end of the ArrayList
	if (index == arrayList->length) {
		ArrayList_pushBack(arrayList, item);
		return;
	}

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	COLLECTIONS_FATAL_IF(!ArrayList_inBounds(arrayList, index),
		COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_INDEX_OOB,
		COLLECTIONS_ERR_ARRAY_LIST, index, COLLECTIONS_ERR_ARRAY_LIST, arrayList->length);

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		ArrayList_resize(arrayList, newCapacity);
	}

	// Move all items after the insertion index forward one
	const size_t itemsToMove = arrayList->length - index;
	const u8* src = (u8*)arrayList->items + index * arrayList->itemSize;
	u8* dest = (u8*)arrayList->items + (index + 1) * arrayList->itemSize;
	if (itemsToMove > 0) {
		Memory_move(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Set the value of the item
	ArrayList_setItem(arrayList, index, item);

	// Increment the length of the ArrayList
	arrayList->length++;
}

void ArrayList_remove(ArrayList* arrayList, const size_t index) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_REMOVE COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	COLLECTIONS_FATAL_IF(!ArrayList_inBounds(arrayList, index),
		COLLECTIONS_ERR_FAILED_REMOVE COLLECTIONS_ERR_INDEX_OOB,
		COLLECTIONS_ERR_ARRAY_LIST, index, COLLECTIONS_ERR_ARRAY_LIST, arrayList->length);

	// Use pop back function if the item should be removed from the end of the ArrayList
	if (index == arrayList->length - 1) {
		ArrayList_popBack(arrayList);
		return;
	}

	// Copy all items after the removed item back one index
	const size_t itemsToMove = arrayList->length - index - 1;
	const u8* src = (u8*)arrayList->items + (index + 1) * arrayList->itemSize;
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	if (itemsToMove > 0) {
		Memory_move(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}

void* ArrayList_getItem(const ArrayList* arrayList, const size_t index) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_GET COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	COLLECTIONS_FATAL_IF(!ArrayList_inBounds(arrayList, index),
		COLLECTIONS_ERR_FAILED_GET COLLECTIONS_ERR_INDEX_OOB,
		COLLECTIONS_ERR_ARRAY_LIST, index, COLLECTIONS_ERR_ARRAY_LIST, arrayList->length);

	// Return a pointer to the item
	return (u8*)arrayList->items + index * arrayList->itemSize;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ArrayList_setItem(ArrayList* arrayList, const size_t index, const void* item) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	COLLECTIONS_FATAL_IF(!ArrayList_inBounds(arrayList, index),
		COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_INDEX_OOB,
		COLLECTIONS_ERR_ARRAY_LIST, index, COLLECTIONS_ERR_ARRAY_LIST, arrayList->length);

	// Throw a fatal error if item is a nullptr
	COLLECTIONS_FATAL_IF(!item,
		COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Item");

	// Copy the item to the ArrayList
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	Memory_copy(dest, item, arrayList->itemSize);
}

void ArrayList_pushFront(ArrayList* arrayList, const void* item) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the item is a nullptr
	COLLECTIONS_FATAL_IF(!item,
		COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Item");

	// Insert the element in the first position
	ArrayList_insert(arrayList, item, 0);
}

void ArrayList_popFront(ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the ArrayList has no items
	COLLECTIONS_FATAL_IF(arrayList->length == 0,
		COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_POP_EMPTY,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Remove the element from the first position
	ArrayList_remove(arrayList, 0);
}

void ArrayList_pushBack(ArrayList* arrayList, const void* item) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the item is a nullptr
	COLLECTIONS_FATAL_IF(!item,
		COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Item");

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		ArrayList_resize(arrayList, newCapacity);
	}

	// Add the item to the ArrayList
	const size_t index = arrayList->length;
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	Memory_copy(dest, item, arrayList->itemSize);
	arrayList->length++;
}

void ArrayList_popBack(ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the ArrayList has no items
	COLLECTIONS_FATAL_IF(arrayList->length == 0,
		COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_POP_EMPTY,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Decrement the length of the ArrayList
	arrayList->length--;
}

void* ArrayList_find(const ArrayList* arrayList, const void* key, const CollectionsComparisonFn compare) {
	// Throw a fatal error if the ArrayList is a nullptr
	COLLECTIONS_FATAL_IF(!arrayList,
		COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, COLLECTIONS_ERR_ARRAY_LIST);

	// Throw a fatal error if the key is a nullptr
	COLLECTIONS_FATAL_IF(!key,
		COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Key");

	// Throw a fatal error if the compare function is a nullptr
	COLLECTIONS_FATAL_IF(!compare,
		COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
		COLLECTIONS_ERR_ARRAY_LIST, "Compare function");
	
	// Iterate through the ArrayList until a match is found
	for (size_t i = 0; i < arrayList->length; i++) {
		void* item = ArrayList_getItem(arrayList, i);
		if (compare(item, key) == 0) {
			return item;
		}
	}

	// Return a nullptr if no match was found
	return nullptr;
}
