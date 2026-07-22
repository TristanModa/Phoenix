#include "arrayList.h"

#include <assert.h>
#include <stdlib.h>

#include "core/core.h"

static void* getItem(ArrayList* arrayList, size_t index);
static void setItem(ArrayList* arrayList, size_t index, const void* item);
static void* insertItem(ArrayList* arrayList, size_t index, const void* item);
static void removeItem(ArrayList* arrayList, size_t index);

ArrayList* ArrayList_create(const size_t itemSize, const size_t capacity,
                            const CollectionsItemDestructorFn itemDestructor) {
	// Return null if the item size is 0
	if (itemSize == 0) {
		Logger_error("Failed to create ArrayList: Item size cannot be 0");
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

	// Return the newly created ArrayList
	return arrayList;
}

bool ArrayList_destroy(ArrayList* arrayList) {
	// Return false if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to destroy ArrayList: ArrayList is null");
		return false;
	}

	// Call the item destructor on each item if it is not null
	if (arrayList->itemDestructor) {
		ArrayList_forEach(arrayList, arrayList->itemDestructor);
	}

	// Release the ArrayList and its items
	Memory_free(arrayList->items);
	Memory_free(arrayList);

	// Return success
	return true;
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

bool ArrayList_clear(ArrayList* arrayList) {
	// Return false if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to clear ArrayList: ArrayList is null");
		return false;
	}

	// Call the item destructor on each item if the destructor is not null
	if (arrayList->itemDestructor) {
		for (size_t i = 0; i < arrayList->length; i++) {
			arrayList->itemDestructor(&arrayList->items[i]);
		}
	}

	// Set the length of the ArrayList to 0
	arrayList->length = 0;

	// Return success
	return true;
}

bool ArrayList_resize(ArrayList* arrayList, const size_t newCapacity) {
	// Return failure if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to resize ArrayList: ArrayList is null");
		return false;
	}

	// Return failure if the new capacity is less than the length
	if (newCapacity < arrayList->length) {
		Logger_error(
			"Failed to resize ArrayList: New capacity (%zu) is less than current length (%zu)",
			newCapacity, arrayList->length);
		return false;
	}

	// Free the buffer and return success if the desired capacity is 0
	if (newCapacity == 0) {
		Memory_free(arrayList->items);
		arrayList->items = nullptr;
		arrayList->capacity = 0;
		return true;
	}

	// Reallocate the ArrayList items
	void* items = Memory_realloc(arrayList->items, arrayList->itemSize * newCapacity);
	if (!items) {
		Logger_error("Failed to resize ArrayList: Memory reallocation failed for ArrayList items");
		return false;
	}

	// Update the ArrayList
	arrayList->capacity = newCapacity;
	arrayList->items = items;

	// Return success
	return true;
}

bool ArrayList_shrink(ArrayList* arrayList) {
	// Return false if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to shrink ArrayList: ArrayList is null");
		return false;
	}

	// Resize the ArrayList
	size_t newCapacity = ArrayList_getLength(arrayList);
	return ArrayList_resize(arrayList, newCapacity);
}

void* ArrayList_getItem(ArrayList* arrayList, const size_t index) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to get item from ArrayList: ArrayList is null");
		return nullptr;
	}

	// Return null if the index is out of bounds
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

bool ArrayList_removeItem(ArrayList* arrayList, const size_t index, void* out) {
	// Return failure if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to remove item from ArrayList: ArrayList is null");
		return false;
	}

	// Return failure if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to remove item from ArrayList: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return false;
	}

	// Copy the item to out, or destroy it if out is null
	void* item = getItem(arrayList, index);
	if (out) memcpy(out, item, arrayList->itemSize);
	else if (arrayList->itemDestructor) arrayList->itemDestructor(item);

	// Remove the item from the ArrayList
	removeItem(arrayList, index);

	// Return success
	return true;
}

void* ArrayList_replaceItem(ArrayList* arrayList, size_t index, const void* newItem, void* out) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to replace ArrayList item: ArrayList is null");
		return nullptr;
	}

	// Return null if the new item is null
	if (!newItem) {
		Logger_error("Failed to replace ArrayList item: New item is null");
		return nullptr;
	}

	// Return null if the index is out of bounds
	if (index >= arrayList->length) {
		Logger_error(
			"Failed to replace ArrayList item: Index %zu is out of bounds for ArrayList of length %zu",
			index, arrayList->length);
		return nullptr;
	}

	// Copy the item to out, or destroy it if out is null
	void* item = getItem(arrayList, index);
	if (out) memcpy(out, item, arrayList->itemSize);
	else if (arrayList->itemDestructor) arrayList->itemDestructor(item);

	// Set the new item
	setItem(arrayList, index, newItem);

	// Return the new item
	return getItem(arrayList, index);
}

void* ArrayList_pushBackItem(ArrayList* arrayList, void* item) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to push back item to ArrayList: ArrayList is null");
		return nullptr;
	}

	// Return null if the item is null
	if (!item) {
		Logger_error("Failed to push back item to ArrayList: Item is null");
		return nullptr;
	}

	// Insert the item at the end of the ArrayList
	return insertItem(arrayList, arrayList->length, item);
}

bool ArrayList_popBackItem(ArrayList* arrayList, void* out) {
	// Return failure if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to pop back item from ArrayList: ArrayList is null");
		return false;
	}

	// Return failure if the ArrayList is empty
	if (arrayList->length == 0) {
		Logger_error("Failed to pop back item from ArrayList: ArrayList is empty");
		return false;
	}

	// Get the index of the back item
	size_t index = arrayList->length - 1;

	// Copy the item to out, or destroy it if out is null
	void* item = getItem(arrayList, index);
	if (out) memcpy(out, item, arrayList->itemSize);
	else if (arrayList->itemDestructor) arrayList->itemDestructor(item);

	// Remove the item
	removeItem(arrayList, index);

	// Return success
	return true;
}

bool ArrayList_forEach(ArrayList* arrayList, const CollectionsForEachActionFn action) {
	// Return failure if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to execute forEach on ArrayList: ArrayList is null");
		return false;
	}

	// Return failure if the action is null
	if (!action) {
		Logger_error("Failed to execute forEach on ArrayList: Action is null");
		return false;
	}

	// Iterate through the ArrayList and execute the action on each item
	for (size_t i = 0; i < arrayList->length; i++) {
		action(&arrayList->items[i]);
	}

	// Return success
	return true;
}

void* ArrayList_find(ArrayList* arrayList, const void* key, const CollectionsCompareFn compare) {
	// Return null if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed find ArrayList item: ArrayList is null");
		return nullptr;
	}

	// Return if the key is null
	if (!key) {
		Logger_error("Failed find ArrayList item: Key is null");
		return nullptr;
	}

	// Return if the compare function is null
	if (!compare) {
		Logger_error("Failed find ArrayList item: Compare function is null");
		return nullptr;
	}

	// Iterate through the ArrayList until a match is found
	void* result = nullptr;
	size_t i = 0;
	while (i < arrayList->length) {
		void *item = &arrayList->items[i];
		if (compare(item, key) == 0) {
			result = item;
			break;
		}

		i++;
	}

	// Return the result
	return result;
}

bool ArrayList_sort(ArrayList* arrayList, CollectionsCompareFn compare) {
	// Return failure if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to sort ArrayList: ArrayList is null");
		return false;
	}

	// Return failure if the action is null
	if (!compare) {
		Logger_error("Failed to sort ArrayList: Action is null");
		return false;
	}

	// Sort the ArrayList by the comparison function
	qsort(arrayList->items, arrayList->length, arrayList->itemSize, compare);

	// Return success
	return true;
}

ArrayListIterator ArrayList_begin(ArrayList* arrayList) {
	// Return an empty iterator if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to create ArrayList iterator: ArrayList is null");
		return (ArrayListIterator){};
	}

	// Return an iterator starting at the beginning of the ArrayList
	return (ArrayListIterator){
		.arrayList = arrayList,
		.currentIndex = 0,
		.lastReturnedIndex = SIZE_MAX
	};
}

ArrayListIterator ArrayList_end(ArrayList* arrayList) {
	// Return an empty iterator if the ArrayList is null
	if (!arrayList) {
		Logger_error("Failed to create ArrayList iterator: ArrayList is null");
		return (ArrayListIterator){};
	}

	// Return an iterator starting at the end of the ArrayList
	return (ArrayListIterator){
		.arrayList = arrayList,
		.currentIndex = arrayList->length,
		.lastReturnedIndex = SIZE_MAX
	};
}

void* ArrayListIterator_next(ArrayListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to advance ArrayListIterator: ArrayListIterator is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->arrayList || iterator->currentIndex > iterator->arrayList->length) {
		Logger_error("Failed to advance ArrayListIterator: ArrayListIterator is invalid");
		return nullptr;
	}

	// Return null if there is no next item
	if (iterator->currentIndex >= iterator->arrayList->length) {
		return nullptr;
	}

	// Get the item
	void* item = getItem(iterator->arrayList, iterator->currentIndex);

	// Update iterator state
	iterator->lastReturnedIndex = iterator->currentIndex;
	iterator->currentIndex++;

	// Return the item
	return item;
}

void* ArrayListIterator_previous(ArrayListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to regress ArrayListIterator: ArrayListIterator is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->arrayList || iterator->currentIndex > iterator->arrayList->length) {
		Logger_error("Failed to regress ArrayListIterator: ArrayListIterator is invalid");
		return nullptr;
	}

	// Return null if there is no previous item
	if (iterator->currentIndex == 0) {
		return nullptr;
	}

	// Get the item
	void* item = getItem(iterator->arrayList, iterator->currentIndex - 1);

	// Update iterator state
	iterator->lastReturnedIndex = iterator->currentIndex;
	iterator->currentIndex--;

	// Return the item
	return item;
}

void* ArrayListIterator_insertItem(ArrayListIterator* iterator, const void* item) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item insertion with ArrayListIterator: "
			"ArrayListIterator is null");
		return nullptr;
	}

	// Return null if the iterator is null
	if (!item) {
		Logger_error(
			"Failed to perform ArrayList item insertion with ArrayListIterator: "
			"Item is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->arrayList || iterator->currentIndex > iterator->arrayList->length) {
		Logger_error(
			"Failed to perform ArrayList item insertion with ArrayListIterator: "
			"ArrayListIterator is invalid");
		return nullptr;
	}

	// Insert the item
	return insertItem(iterator->arrayList, iterator->currentIndex++, item);
}

bool ArrayListIterator_removeItem(ArrayListIterator* iterator, void* out) {
	// Return failure if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item removal with ArrayListIterator: "
			"ArrayListIterator is null");
		return false;
	}

	// Return failure if the iterator is in an invalid state
	if (!iterator->arrayList || iterator->currentIndex > iterator->arrayList->length) {
		Logger_error(
			"Failed to perform ArrayList item removal with ArrayListIterator: "
			"ArrayListIterator is invalid");
		return false;
	}

	// Return failure if the last returned index is invalid
	if (iterator->lastReturnedIndex == SIZE_MAX) {
		Logger_error(
			"Failed to perform LinkedList item removal with LinkedListIterator: "
			"No item has been traversed yet, or the item has already been removed");
		return false;
	}

	// Copy the item to out, or destroy it if out is null
	void* item = getItem(iterator->arrayList, iterator->lastReturnedIndex);
	if (out) memcpy(out, item, iterator->arrayList->itemSize);
	else if (iterator->arrayList->itemDestructor) iterator->arrayList->itemDestructor(item);

	// Remove the item from the ArrayList
	removeItem(iterator->arrayList, iterator->lastReturnedIndex);

	// Update iterator state
	if (iterator->currentIndex > iterator->lastReturnedIndex) iterator->currentIndex--;
	iterator->lastReturnedIndex = SIZE_MAX;

	// Return success
	return true;
}

void* ArrayListIterator_replaceItem(ArrayListIterator* iterator, const void* newItem, void* out) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform ArrayList item replacement with ArrayListIterator: "
			"ArrayListIterator is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->arrayList || iterator->currentIndex > iterator->arrayList->length) {
		Logger_error(
			"Failed to perform ArrayList item replacement with ArrayListIterator: "
			"ArrayListIterator is invalid");
		return nullptr;
	}

	// Return null if the last returned index is invalid
	if (iterator->lastReturnedIndex == SIZE_MAX) {
		Logger_error(
			"Failed to perform LinkedList item replacement with LinkedListIterator: "
			"No item has been traversed yet, or the item has been removed");
		return nullptr;
	}

	// Replace the item
	return ArrayList_replaceItem(iterator->arrayList, iterator->lastReturnedIndex, newItem, out);
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

void removeItem(ArrayList* arrayList, const size_t index) {
	assert(arrayList);
	assert(index < arrayList->length);

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