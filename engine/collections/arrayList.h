#ifndef ENGINE_COLLECTIONS_ARRAYLIST_H
#define ENGINE_COLLECTIONS_ARRAYLIST_H

#include <stddef.h>

#include "common.h"

/**
 * A contiguous dynamically sized array.
 * All fields are managed by the set of ArrayList functions and should never be modified directly.
 */
typedef struct arrayList {
	/**
	 * The size of an item in bytes
	 */
	size_t itemSize;
	/**
	 * The number of items in ArrayList
	 */
	size_t length;
	/**
	 * The maximum number of items that the ArrayList can hold
	 */
	size_t capacity;
	/**
	 * A pointer to the items of the ArrayList
	 */
	void* items;
	/**
	 * The destructor to use when destroying ArrayList items
	 */
	CollectionsItemDestructorFn itemDestructor;
} ArrayList;

/**
 * An iterator for an ArrayList.
 * All fields are managed by the set of ArrayListIterator functions and should never be modified directly.
 */
typedef struct arrayListIterator {
	/**
	 * The associated ArrayList of the iterator
	 */
	ArrayList* arrayList;
	/**
	 * The current index of the iterator
	 */
	size_t currentIndex;
	/**
	 * The last returned index of the iterator
	 */
	size_t lastReturnedIndex;
} ArrayListIterator;

/**
 * Initializes an ArrayList
 * @param itemSize The size of an item in bytes. Must be greater than 0.
 * @param capacity The initial capacity of the ArrayList
 * @param itemDestructor The destructor to use when destroying ArrayList items, or null for no destructor
 * @return A pointer to the newly created ArrayList.
 * The caller owns the returned object and must free it using ArrayList_destroy().
 */
ArrayList* ArrayList_create(size_t itemSize, size_t capacity, CollectionsItemDestructorFn itemDestructor);
/**
 * Destroys an ArrayList and releases all associated memory
 * @param arrayList The ArrayList to destroy
 * @returns True on success
 */
bool ArrayList_destroy(ArrayList* arrayList);

/**
 * Gets the length of an ArrayList
 * @param arrayList The ArrayList to get the length of
 * @return The length of the ArrayList, or 0 on failure
 */
size_t ArrayList_getLength(const ArrayList* arrayList);
/**
 * Gets the capacity of an ArrayList
 * @param arrayList The ArrayList to get the capacity of
 * @return The capacity of the ArrayList, or 0 on failure
 */
size_t ArrayList_getCapacity(const ArrayList* arrayList);

/**
 * Destroys all items in an ArrayList
 * @param arrayList The ArrayList to clear
 * @return
 */
bool ArrayList_clear(ArrayList* arrayList);
/**
 * Sets the capacity of an ArrayList
 * @param arrayList The ArrayList to resize
 * @param newCapacity The new capacity of the ArrayList after resizing.
 * Must be greater than its current length.
 * @return True on success
 */
bool ArrayList_resize(ArrayList* arrayList, size_t newCapacity);
/**
 * Resizes an ArrayList such that its capacity is equal to its length
 * @param arrayList The ArrayList to resize
 * @return True on success
 */
bool ArrayList_shrink(ArrayList* arrayList);

/**
 * Gets an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to get the item from
 * @param index The index of the item to get
 * @return A pointer to the item, or null on failure
 */
void* ArrayList_getItem(ArrayList* arrayList, size_t index);

/**
 * Inserts a new item to an ArrayList at the specified index.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param arrayList The ArrayList to insert to
 * @param index The index to insert the item at
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_insertItem(ArrayList* arrayList, size_t index, void* item);
/**
 * Removes an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to remove from
 * @param index The index to remove the item from
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool ArrayList_removeItem(ArrayList* arrayList, size_t index, void* out);
/**
 * Replaces an item at the specified index of an ArrayList with another.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param arrayList The ArrayList containing the item to replace
 * @param index The index of the item to replace
 * @param newItem The item to insert
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_replaceItem(ArrayList* arrayList, size_t index, const void* newItem, void* out);

/**
 * Inserts a new item to the end of an ArrayList.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param arrayList The ArrayList to insert to
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_pushBackItem(ArrayList* arrayList, void* item);
/**
 * Removes the item at the end of an ArrayList
 * @param arrayList The ArrayList to remove from
 * @param out A pointer to the location to store the removed item.
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool ArrayList_popBackItem(ArrayList *arrayList, void *out);

/**
 * Executes an action on each item of an ArrayList
 * @param arrayList The ArrayList to iterate
 * @param action The action to execute
 * @return True on success
 */
bool ArrayList_forEach(ArrayList* arrayList, CollectionsForEachActionFn action);
/**
 * Finds the first instance of an item that matches the specified key in an ArrayList
 * @param arrayList The ArrayList to search
 * @param key The object to compare items against
 * @param compare The function to use to compare the items
 * @return The matching item, or null on failure
 */
void* ArrayList_find(ArrayList* arrayList, const void* key, CollectionsCompareFn compare);
/**
 * Sorts an ArrayList according to a comparison function
 * @param arrayList The ArrayList to sort
 * @param compare The comparison function to use
 * @return True on success
 */
bool ArrayList_sort(ArrayList* arrayList, CollectionsCompareFn compare);

/**
 * Creates an ArrayListIterator pointing to the start of an ArrayList
 * @param arrayList The ArrayList to create the ArrayListIterator with
 * @return An ArrayListIterator pointing to the start of the ArrayList
 */
ArrayListIterator ArrayList_begin(ArrayList* arrayList);
/**
 * Creates an ArrayListIterator pointing to the end of an ArrayList
 * @param arrayList The ArrayList to create the ArrayListIterator with
 * @return An ArrayListIterator pointing to the end of the ArrayList
 */
ArrayListIterator ArrayList_end(ArrayList* arrayList);

/**
 * Advances an ArrayListIterator
 * @param iterator The ArrayListIterator to advance
 * @return The next item in the ArrayList, or null if no next item exists
 */
void* ArrayListIterator_next(ArrayListIterator* iterator);
/**
 * Regresses an ArrayListIterator
 * @param iterator The ArrayListIterator to regress
 * @return The previous item in the ArrayList, or null if no previous item exists
 */
void* ArrayListIterator_previous(ArrayListIterator* iterator);

/**
 * Inserts a new item to an ArrayList at the position of an ArrayListIterator.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param iterator The ArrayListIterator to insert at
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayListIterator_insertItem(ArrayListIterator* iterator, const void* item);
/**
 * Removes an item from an ArrayList at the position of an ArrayListIterator.
 * @param iterator The ArrayListIterator to remove at
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool ArrayListIterator_removeItem(ArrayListIterator* iterator, void* out);
/**
 * Replaces an ArrayList item at position of an ArrayListIterator with another.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param iterator The ArrayList containing the item to replace
 * @param newItem The item to insert
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayListIterator_replaceItem(ArrayListIterator* iterator, const void* newItem, void* out);

#endif //ENGINE_COLLECTIONS_ARRAYLIST_H