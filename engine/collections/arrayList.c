#include "arrayList.h"

#include <stdlib.h>

#include "core/core.h"

ArrayList* ArrayList_create(const size_t itemSize, const size_t capacity) {
	// Throw a fatal error if the item size is zero
	if (itemSize == 0) {
		Logger_fatal("Failed to create ArrayList: Item size cannot be 0.");
		exit(EXIT_FAILURE);
	}

	// Allocate memory for the ArrayList struct
	ArrayList* arrayList = Memory_malloc(sizeof(*arrayList));
	if (!arrayList) {
		Logger_fatal("Failed to create ArrayList: Memory allocation for ArrayList instance failed.");
		exit(EXIT_FAILURE);
	}

	// Initialize ArrayList fields
	arrayList->itemSize = itemSize;
	arrayList->capacity = capacity;
	arrayList->length = 0;

	// Allocate memory for the ArrayList items
	arrayList->items = capacity != 0 ? Memory_malloc(itemSize * capacity) : nullptr;
	if (!arrayList->items && capacity != 0) {
		Logger_fatal("Failed to create ArrayList: Memory allocation for ArrayList items failed.");
		Memory_free(arrayList);
		return nullptr;
	}

	return arrayList;
}

void ArrayList_destroy(ArrayList* arrayList) {
	Memory_free(arrayList->items);
	Memory_free(arrayList);
}

size_t ArrayList_getLength(const ArrayList* arrayList) {
	return arrayList->length;
}

bool ArrayList_inBounds(const ArrayList* arrayList, const size_t index) {
	return index < arrayList->length;
}

void ArrayList_clear(ArrayList* arrayList) {
	arrayList->length = 0;
}

void ArrayList_resize(ArrayList* arrayList, const size_t capacity) {
	// Return if the new capacity is less than the length
	if (capacity < arrayList->length) {
		Logger_error(
			"Failed to resize Array List: New capacity (%zu) is not large enough to contain %zu items.",
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
	if (!items) {
		Logger_fatal("Failed to resize Array List: Memory reallocation failed.");
		exit(EXIT_FAILURE);
	}

	// Update the ArrayList
	arrayList->capacity = capacity;
	arrayList->items = items;
}

void ArrayList_insert(ArrayList* arrayList, const void* item, const size_t index) {
	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal("Failed to insert item to ArrayList: Item is a nullptr.");
		exit(EXIT_FAILURE);
	}

	// Use push back function if the item should be added to the end of the ArrayList
	if (index == arrayList->length) {
		ArrayList_pushBack(arrayList, item);
		return;
	}

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	if (!ArrayList_inBounds(arrayList, index)) {
		Logger_fatal(
			"Failed to insert item to ArrayList: Index %zu out of bounds for ArrayList of length %zu.",
			index, arrayList->length);
		exit(EXIT_FAILURE);
	}

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
		memmove(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Set the value of the item
	ArrayList_setItem(arrayList, index, item);

	// Increment the length of the ArrayList
	arrayList->length++;
}

void ArrayList_remove(ArrayList* arrayList, const size_t index) {
	// Throw a fatal error if the index is outside the bounds of the ArrayList
	if (!ArrayList_inBounds(arrayList, index)) {
		Logger_fatal(
			"Failed to remove ArrayList item: Index %zu out of bounds for ArrayList of length %zu.",
			index, arrayList->length);
	}

	// Use pop back function if the item should be removed from the end of the ArrayList
	if (index == arrayList->length) {
		ArrayList_popBack(arrayList);
		return;
	}

	// Throw a fatal error if the index is outside the bounds of the ArrayList
	if (!ArrayList_inBounds(arrayList, index)) {
		Logger_fatal(
			"Failed to remove item from ArrayList: Index %zu out of bounds for ArrayList of length %zu.",
			index, arrayList->length);
		exit(EXIT_FAILURE);
	}

	// Copy all items after the removed item back one index
	const size_t itemsToMove = arrayList->length - index - 1;
	const u8* src = (u8*)arrayList->items + (index + 1) * arrayList->itemSize;
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	if (itemsToMove > 0) {
		memmove(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}

void* ArrayList_getItem(const ArrayList* arrayList, const size_t index) {
	// Throw a fatal error if the index is outside the bounds of the ArrayList
	if (ArrayList_inBounds(arrayList, index)) {
		Logger_fatal(
			"Failed to get ArrayList item: Index %zu out of bounds for ArrayList of length %zu.",
			index, arrayList->length);
		exit(EXIT_FAILURE);
	}

	return (u8*)arrayList->items + index * arrayList->itemSize;
}

void ArrayList_setItem(const ArrayList* arrayList, const size_t index, const void* item) {
	// Throw a fatal error if the index is outside the bounds of the ArrayList
	if (ArrayList_inBounds(arrayList, index)) {
		Logger_fatal(
			"Failed to set ArrayList item: Index %zu out of bounds for ArrayList of length %zu.",
			index, arrayList->length);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if item is a nullptr
	if (!item) {
		Logger_fatal("Failed to set ArrayList item: Item is a nullptr.");
		exit(EXIT_FAILURE);
	}

	// Copy the item to the ArrayList
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	memcpy(dest, item, arrayList->itemSize);
}

void ArrayList_pushFront(ArrayList* arrayList, const void* item) {
	ArrayList_insert(arrayList, item, 0);
}

void ArrayList_popFront(ArrayList* arrayList) {
	ArrayList_remove(arrayList, 0);
}

void ArrayList_pushBack(ArrayList* arrayList, const void* item) {
	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal("Failed to push back item to ArrayList: Item is a nullptr.");
		exit(EXIT_FAILURE);
	}

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		ArrayList_resize(arrayList, newCapacity);
	}

	// Add the item to the ArrayList
	const size_t index = arrayList->length;
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	memcpy(dest, item, arrayList->itemSize);
	arrayList->length++;
}

void ArrayList_popBack(ArrayList* arrayList) {
	// Throw a fatal error if the ArrayList is empty
	if (arrayList->length == 0) {
		Logger_fatal("Failed to pop back item from ArrayList: Cannot pop from an ArrayList of length 0.");
		exit(EXIT_FAILURE);
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}