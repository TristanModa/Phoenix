#include "arrayList.h"

#include "core/core.h"

ArrayList* ArrayList_create(const size_t itemSize, const size_t capacity) {
	// Return a nullptr if the item size is zero
	if (itemSize == 0) {
		return nullptr;
	}

	// Allocate memory for the ArrayList struct
	ArrayList* arrayList = Memory_malloc(sizeof(*arrayList));
	if (!arrayList) {
		return nullptr;
	}

	// Initialize ArrayList fields
	arrayList->itemSize = itemSize;
	arrayList->capacity = capacity;
	arrayList->length = 0;

	// Allocate memory for the ArrayList items
	arrayList->items = capacity != 0 ? Memory_malloc(itemSize * capacity) : nullptr;
	if (!arrayList->items && capacity != 0) {
		Memory_free(arrayList);
		return nullptr;
	}

	return arrayList;
}

void ArrayList_destroy(ArrayList* arrayList) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return;
	}

	// Release the ArrayList and its items
	Memory_free(arrayList->items);
	Memory_free(arrayList);
}

size_t ArrayList_getLength(const ArrayList* arrayList) {
	// Return 0 if the ArrayList is a nullptr
	if (!arrayList) {
		return 0;
	}

	// Return the length of the ArrayList
	return arrayList->length;
}

size_t ArrayList_getCapacity(const ArrayList* arrayList) {
	
}

void ArrayList_clear(ArrayList* arrayList) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return;
	}

	// Set the length of the ArrayList to zero
	arrayList->length = 0;
}

bool ArrayList_resize(ArrayList* arrayList, const size_t capacity) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return false;
	}

	// Return if the new capacity is less than the length
	if (capacity < arrayList->length) {
		return false;
	}

	// Free the buffer if the desired capacity is zero
	if (capacity == 0) {
		Memory_free(arrayList->items);
		arrayList->items = nullptr;
		arrayList->capacity = 0;
		return true;
	}

	// Reallocate the ArrayList items
	void* items = Memory_realloc(arrayList->items, arrayList->itemSize * capacity);
	if (!items) {
		return false;
	}

	// Update the ArrayList
	arrayList->capacity = capacity;
	arrayList->items = items;

	// Return success
	return true;
}

void* ArrayList_insert(ArrayList* arrayList, const void* item, const size_t index) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index > arrayList->length) {
		return nullptr;
	}

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		if (!ArrayList_resize(arrayList, newCapacity)) {
			return nullptr;
		}
	}

	// Move all items after the insertion index forward one
	const size_t itemsToMove = arrayList->length - index;
	const u8* src = (u8*)arrayList->items + index * arrayList->itemSize;
	u8* dest = (u8*)arrayList->items + (index + 1) * arrayList->itemSize;
	if (itemsToMove > 0) {
		Memory_move(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Set the value of the item
	dest = (u8*)arrayList->items + index * arrayList->itemSize;
	Memory_copy(dest, item, arrayList->itemSize);

	// Increment the length of the ArrayList
	arrayList->length++;

	// Return the inserted item
	return dest;
}

void ArrayList_remove(ArrayList* arrayList, const size_t index) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
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

void* ArrayList_getItem(ArrayList* arrayList, const size_t index) {
	// Return a nullptr if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		return nullptr;
	}

	// Return a pointer to the item
	return (u8*)arrayList->items + index * arrayList->itemSize;
}

void* ArrayList_setItem(ArrayList* arrayList, const size_t index, const void* item) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		return nullptr;
	}

	// Copy the item to the ArrayList
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	Memory_copy(dest, item, arrayList->itemSize);

	// Return the item
	return dest;
}

void* ArrayList_pushFront(ArrayList* arrayList, const void* item) {
	// Return a nullptr if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return a nullptr if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Insert the element in the first position
	return ArrayList_insert(arrayList, item, 0);
}

void ArrayList_popFront(ArrayList* arrayList) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return;
	}

	// Return if the ArrayList has no items
	if (arrayList->length == 0) {
		return;
	}

	// Remove the element from the first position
	ArrayList_remove(arrayList, 0);
}

void* ArrayList_pushBack(ArrayList* arrayList, const void* item) {
	// Return a nullptr if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return a nullptr if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		if (!ArrayList_resize(arrayList, newCapacity)) {
			return nullptr;
		}
	}

	// Add the item to the ArrayList
	const size_t index = arrayList->length;
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	Memory_copy(dest, item, arrayList->itemSize);
	arrayList->length++;

	// Return the item
	return dest;
}

void ArrayList_popBack(ArrayList* arrayList) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return;
	}

	// Return if the ArrayList has no items
	if (arrayList->length == 0) {
		return;
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}

void* ArrayList_find(ArrayList* arrayList, const void* key, const ArrayListComparisonFn compare) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		return nullptr;
	}

	// Return if the key is a nullptr
	if (!key) {
		return nullptr;
	}

	// Return if the compare function is a nullptr
	if (!compare) {
		return nullptr;
	}

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
