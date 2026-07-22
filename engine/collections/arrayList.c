#include "arrayList.h"

#include <assert.h>

#include "core/core.h"

static void* getItem(ArrayList* arrayList, size_t index);
static void setItem(ArrayList* arrayList, size_t index, const void* item);
static void* insertItem(ArrayList* arrayList, size_t index, const void* item);
static void removeItem(ArrayList* arrayList, size_t index, void** item);
static void destroyItem(ArrayList* arrayList, size_t index);

ArrayList* ArrayList_create(const size_t itemSize, const size_t capacity,
                            const CollectionsItemDestructorFn itemDestructor) {
	// Return null if the item size is zero
	if (itemSize == 0) {
		Logger_error("Failed to create ArrayList: Item size cannot be zero.");
		return nullptr;
	}

	// Allocate memory for the ArrayList instance
	ArrayList* arrayList = Memory_malloc(sizeof(*arrayList));
	if (!arrayList) {
		Logger_error("Failed to create ArrayList: Memory allocation failed for ArrayList instance");
		return nullptr;
	}

	// Initialize ArrayList fields
	arrayList->itemSize = itemSize;
	arrayList->capacity = capacity;
	arrayList->length = 0;
	arrayList->itemDestructor = itemDestructor;

	// Allocate memory for ArrayList items
	arrayList->items = capacity != 0 ? Memory_malloc(itemSize * capacity) : nullptr;
	if (!arrayList->items && capacity != 0) {
		Logger_error("Failed to create ArrayList: Memory allocation failed for ArrayList items");
		Memory_free(arrayList);
		return nullptr;
	}

	return arrayList;
}

void ArrayList_destroy(ArrayList* arrayList) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to destroy ArrayList: ArrayList is null");
		return;
	}

	// Call the item destructor on each item if it is not null
	if (arrayList->itemDestructor) {
		ArrayList_forEach(arrayList, arrayList->itemDestructor);
	}

	// Release the ArrayList and its items
	Memory_free(arrayList->items);
	Memory_free(arrayList);
}

size_t ArrayList_getLength(const ArrayList* arrayList) {
	// Return 0 if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to get length of ArrayList: ArrayList is null");
		return 0;
	}

	// Return the length of the ArrayList
	return arrayList->length;
}

size_t ArrayList_getCapacity(const ArrayList* arrayList) {
	// Return 0 if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to get capacity of ArrayList: ArrayList is null");
		return 0;
	}

	// Return the length of the ArrayList
	return arrayList->capacity;
}

void ArrayList_clear(ArrayList* arrayList) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to clear ArrayList: ArrayList is null");
		return;
	}

	// Call the item destructor on each item if it is not null
	if (arrayList->itemDestructor) {
		ArrayListIterator it = ArrayList_begin(arrayList);
		void* item;
		while ((item = ArrayListIterator_next(&it))) {
			arrayList->itemDestructor(item);
		}
	}

	// Set the length of the ArrayList to zero
	arrayList->length = 0;
}

void ArrayList_resize(ArrayList* arrayList, const size_t capacity) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to resize ArrayList: ArrayList is null");
		return;
	}

	// Return if the new capacity is less than the length
	if (capacity < arrayList->length) {
		Logger_error(
			"Failed to resize ArrayList: New capacity (%zu) is less than current length (%zu)",
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
		Logger_error("Failed to resize ArrayList: Memory reallocation failed for ArrayList items");
		return;
	}

	// Update the ArrayList
	arrayList->capacity = capacity;
	arrayList->items = items;
}

void* ArrayList_getItem(ArrayList* arrayList, const size_t index) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to get item from ArrayList: ArrayList is null");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to get item from ArrayList: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return nullptr;
	}

	// Return a pointer to the item
	return getItem(arrayList, index);
}

void* ArrayList_insertItem(ArrayList* arrayList, const size_t index, void* item) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to insert item to ArrayList: ArrayList is null");
		return nullptr;
	}

	// Return null if the item is null
	if (!item) {
		Logger_error("Failed to insert item to ArrayList: Item is null");
		return nullptr;
	}

	// Return null if the index is out of bounds
	if (index > arrayList->length) {
		Logger_error(
			"Failed to insert item to ArrayList: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return nullptr;
	}

	// Insert the item
	return insertItem(arrayList, index, item);
}

void* ArrayList_removeItem(ArrayList* arrayList, const size_t index) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to remove item from ArrayList: ArrayList is null");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to remove item from ArrayList: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return nullptr;
	}

	// Copy the item to a new memory location
	void* item = Memory_malloc(arrayList->itemSize);
	if (!item) {
		Logger_error("Failed to remove item from ArrayList: Memory allocation failed for removed item");
		return nullptr;
	}
	memcpy(item, getItem(arrayList, index), arrayList->itemSize);

	// Remove the item
	removeItem(arrayList, index, item);
	return item;
}

void ArrayList_destroyItem(ArrayList* arrayList, const size_t index) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to destroy ArrayList item: ArrayList is null");
		return;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to destroy ArrayList item: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return;
	}

	// Destroy the item
	destroyItem(arrayList, index);
}

void* ArrayList_pushBackItem(ArrayList* arrayList, void* item) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to push back item to ArrayList: ArrayList is null");
		return nullptr;
	}

	// Insert the item at the end of the ArrayList
	return ArrayList_insertItem(arrayList, ArrayList_getLength(arrayList), item);
}

void* ArrayList_popBackItem(ArrayList* arrayList) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to pop back item from ArrayList: ArrayList is null");
		return nullptr;
	}

	// Destroy the item at the end of the ArrayList
	return ArrayList_removeItem(arrayList, ArrayList_getLength(arrayList) - 1);
}

void ArrayList_destroyBackItem(ArrayList* arrayList) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to destroy back item of ArrayList: ArrayList is null");
		return;
	}

	// Destroy the item at the end of the ArrayList
	ArrayList_destroyItem(arrayList, ArrayList_getLength(arrayList) - 1);
}

void* ArrayList_replaceItem(ArrayList* arrayList, const void* newItem, const size_t index) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to replace ArrayList item: ArrayList is null");
		return nullptr;
	}

	// Return if the item is null
	if (!newItem) {
		Logger_error("Failed to replace ArrayList item: Item is null");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to replace ArrayList item: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return nullptr;
	}

	// Copy the old item to a new memory location
	void* oldItem = Memory_malloc(arrayList->itemSize);
	if (!oldItem) {
		Logger_error("Failed to replace ArrayList item: Memory allocation failed for removed item");
		return nullptr;
	}
	memcpy(oldItem, getItem(arrayList, index), arrayList->itemSize);

	// Insert the new item
	setItem(arrayList, index, newItem);

	// Return the old item
	return oldItem;
}

void ArrayList_forEach(ArrayList* arrayList, const CollectionsForEachActionFn action) {
	// Return if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to execute forEach on ArrayList: ArrayList is null");
		return;
	}

	// Return if the action is null
	if (!action) {
		Logger_error("Failed to execute forEach on ArrayList: Action is null");
		return;
	}

	// Iterate through the ArrayList
	ArrayListIterator it = ArrayList_begin(arrayList);
	void* item;
	while ((item = ArrayListIterator_next(&it))) {
		action(item);
	}
}

void* ArrayList_find(ArrayList* arrayList, const void* key, const CollectionsCompareFn compare) {
	// Return if the ArrayList is a nullptr
	if (!arrayList) {
		Logger_error("Failed find ArrayList item: ArrayList is null");
		return nullptr;
	}

	// Return if the key is a nullptr
	if (!key) {
		Logger_error("Failed find ArrayList item: Key is null");
		return nullptr;
	}

	// Return if the compare function is a nullptr
	if (!compare) {
		Logger_error("Failed find ArrayList item: Compare function is null");
		return nullptr;
	}

	// Iterate through the ArrayList
	ArrayListIterator it = ArrayList_begin(arrayList);
	void* item;
	while ((item = ArrayListIterator_next(&it))) {
		// Return the item if it matches the key
		if (compare(item, key) == 0) {
			return item;
		}
	}

	// Return a nullptr if no match was found
	return nullptr;
}

ArrayListIterator ArrayList_begin(ArrayList* arrayList) {
	// Return an empty iterator if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to create ArrayList iterator: ArrayList is null");
		return (ArrayListIterator){};
	}

	// Return an iterator starting at the beginning of the ArrayList
	return (ArrayListIterator){ .arrayList = arrayList, .currentIndex = 0 };
}

ArrayListIterator ArrayList_end(ArrayList* arrayList) {
	// Return an empty iterator if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to create ArrayList iterator: ArrayList is null");
		return (ArrayListIterator){};
	}

	// Return an iterator starting at the end of the ArrayList
	return (ArrayListIterator){ .arrayList = arrayList, .currentIndex = arrayList->length };
}

void* ArrayListIterator_next(ArrayListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to advance ArrayListIterator: ArrayListIterator is null");
		return nullptr;
	}

	// Return null if there is no next item
	if (iterator->currentIndex >= iterator->arrayList->length) {
		return nullptr;
	}

	// Return the item
	return getItem(iterator->arrayList, iterator->currentIndex++);
}

void* ArrayListIterator_previous(ArrayListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to regress ArrayListIterator: ArrayListIterator is null");
		return nullptr;
	}

	// Return null if there is no previous item
	if (iterator->currentIndex == 0) {
		return nullptr;
	}

	// Return the item
	return getItem(iterator->arrayList, --iterator->currentIndex);
}

void* ArrayListIterator_insertItem(ArrayListIterator* iterator, void* item) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item insertion with ArrayListIterator: "
			"ArrayListIterator is null");
		return nullptr;
	}

	// Insert the item
	return insertItem(iterator->arrayList, iterator->currentIndex++, item);
}

void* ArrayListIterator_removeItem(ArrayListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item removal with ArrayListIterator: "
			"ArrayListIterator is null");
		return nullptr;
	}

	// Copy the item to a new memory location
	void* item = Memory_malloc(iterator->arrayList->itemSize);
	if (!item) {
		Logger_error(
			"Failed to perform ArrayList item removal with ArrayListIterator: "
			"Memory allocation failed for removed item");
		return nullptr;
	}

	// Remove the item
	removeItem(iterator->arrayList, iterator->currentIndex--, item);
	return item;
}

void ArrayListIterator_destroyItem(ArrayListIterator* iterator) {
	// Return if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item destruction with ArrayListIterator: "
			"ArrayListIterator is null");
		return;
	}

	// Destroy the item
	destroyItem(iterator->arrayList, iterator->currentIndex--);
}

void* getItem(ArrayList* arrayList, const size_t index) {
	assert(arrayList);
	assert(index < arrayList->length);
	return (u8*)arrayList->items + index * arrayList->itemSize;
}

void setItem(ArrayList* arrayList, const size_t index, const void* item) {
	assert(arrayList);
	assert(item);
	assert(index < arrayList->length);
	u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
	memcpy(dest, item, arrayList->itemSize);
}

void* insertItem(ArrayList* arrayList, const size_t index, const void* item) {
	assert(arrayList);
	assert(item);
	assert(index <= arrayList->length);

	// Resize the ArrayList if capacity is reached
	if (arrayList->length == arrayList->capacity) {
		const size_t newCapacity = arrayList->capacity > 0 ? arrayList->capacity * 2 : 1;
		ArrayList_resize(arrayList, newCapacity);
	}

	// Move all items after the insertion index forward one
	const size_t itemsToMove = arrayList->length - index;
	if (itemsToMove > 0) {
		const u8* src = (u8*)arrayList->items + index * arrayList->itemSize;
		u8* dest = (u8*)arrayList->items + (index + 1) * arrayList->itemSize;
		memmove(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Increment the length of the ArrayList
	arrayList->length++;

	// Set the value of the item
	setItem(arrayList, index, item);

	// Return the inserted item
	return getItem(arrayList, index);
}

void removeItem(ArrayList* arrayList, const size_t index, void** item) {
	assert(arrayList);
	assert(item);
	assert(*item);
	assert(index < arrayList->length);

	// Move the item into
	memcpy(item, getItem(arrayList, index), arrayList->itemSize);

	// Move all items after the removed item back one index
	const size_t itemsToMove = arrayList->length - index - 1;
	if (itemsToMove > 0) {
		const u8* src = getItem(arrayList, index + 1);
		u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
		memmove(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}

void destroyItem(ArrayList* arrayList, const size_t index) {
	assert(arrayList);
	assert(index < arrayList->length);

	// Get the item and call its item destructor if it exists
	void* item = getItem(arrayList, index);
	if (arrayList->itemDestructor) {
		arrayList->itemDestructor(item);
	}

	// Move all items after the destroyed item back one index
	const size_t itemsToMove = arrayList->length - index - 1;
	if (itemsToMove > 0) {
		const u8* src = getItem(arrayList, index + 1);
		u8* dest = (u8*)arrayList->items + index * arrayList->itemSize;
		memmove(dest, src, itemsToMove * arrayList->itemSize);
	}

	// Decrement the length of the ArrayList
	arrayList->length--;
}