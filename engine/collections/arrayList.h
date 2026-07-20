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
	 * The number of items in ArrayList
	 */
	size_t length;
	/**
	 * The maximum number of items that the ArrayList can hold
	 */
	size_t capacity;
	/**
	 * The size of an item in bytes
	 */
	size_t itemSize;
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
} ArrayListIterator;

/**
 * Allocates and initializes an ArrayList
 * @param itemSize The size of an item in bytes. Must be greater than zero.
 * @param capacity The initial capacity of the ArrayList
 * @param itemDestructor The destructor to use when destroying ArrayList items, or null for no destructor
 * @return A pointer to the newly created ArrayList. The caller owns the returned object and must free it using ArrayList_destroy().
 */
ArrayList* ArrayList_create(size_t itemSize, size_t capacity, CollectionsItemDestructorFn itemDestructor);
/**
 * Destroys an ArrayList and releases all associated memory
 * @param arrayList The ArrayList to destroy
 */
void ArrayList_destroy(ArrayList* arrayList);

/**
 * Gets the length of an ArrayList
 * @param arrayList The ArrayList to get the length of
 * @return The length of the ArrayList
 */
size_t ArrayList_getLength(const ArrayList* arrayList);
/**
 * Gets the capacity of an ArrayList
 * @param arrayList The ArrayList to get the capacity of
 * @return The capacity of the ArrayList
 */
size_t ArrayList_getCapacity(const ArrayList* arrayList);

/**
 * Destroys all ArrayList items and sets the length of the ArrayList to zero
 * @param arrayList The ArrayList to clear
 */
void ArrayList_clear(ArrayList* arrayList);
/**
 * Sets the capacity of an ArrayList
 * @param arrayList The ArrayList to resize
 * @param capacity The desired capacity to resize to. Must be greater than its current length.
 */
void ArrayList_resize(ArrayList* arrayList, size_t capacity);

/**
 * Gets an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to get the item from
 * @param index The index of the item to get
 * @return A pointer to the item, or null on failure
 */
void* ArrayList_getItem(ArrayList* arrayList, size_t index);

/**
 * Inserts a new item into an ArrayList at the specified index.
 * The item data is shallow copied into the ArrayList's internal storage.
 * Inserting an item invalidates all existing iterators.
 * @param arrayList The ArrayList to insert to
 * @param item The item to insert
 * @param index The index to insert the item at
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_insertItem(ArrayList* arrayList, const void* item, size_t index);
/**
 * Removes an item from the specified index of an ArrayList.
 * Removing an item invalidates all existing iterators.
 * @param arrayList The ArrayList to remove from
 * @param index The index to remove the item from
 * @return A pointer to the removed item, or null on failure.
 * The caller owns the returned object and is responsible for freeing it.
 */
void* ArrayList_removeItem(ArrayList* arrayList, size_t index);
/**
 * Destroys and removes an item from the specified index of an ArrayList.
 * Destroying an item invalidates all existing iterators.
 * @param arrayList The ArrayList to remove from
 * @param index The index to remove the item from
 */
void ArrayList_destroyItem(ArrayList* arrayList, size_t index);
/**
 * Replaces the item at the specified index of an ArrayList with a new item.
 * The item data is shallow copied into the ArrayList's internal storage.
 * @param arrayList The ArrayList of the item to replace
 * @param newItem The new item to replace the old item with
 * @param index The index of the item to replace
 * @return A pointer to the old item or null on failure.
 * The caller owns the returned object and is responsible for freeing it.
 */
void* ArrayList_replaceItem(ArrayList* arrayList, const void* newItem, size_t index);

/**
 * Executes an action on each item of an ArrayList
 * @param arrayList The ArrayList to iterate
 * @param action The action to execute
 */
void ArrayList_forEach(ArrayList* arrayList, CollectionsForEachActionFn action);
/**
 * Finds the first instance of an item that matches key in an ArrayList
 * @param arrayList The ArrayList to search
 * @param key The object to compare elements against
 * @param compare The function to use to compare the elements
 * @return The matching item, or nullptr on failure
 */
void* ArrayList_find(ArrayList* arrayList, const void* key, CollectionsCompareFn compare);

/**
 * Creates an ArrayListIterator pointing to the start of an ArrayList
 * @param arrayList The ArrayList to create the iterator with
 * @return An ArrayListIterator pointing to the start of the ArrayList
 */
ArrayListIterator ArrayList_begin(ArrayList* arrayList);
/**
 * Creates an ArrayListIterator pointing to the end of an ArrayList
 * @param arrayList The ArrayList to create the iterator with
 * @return An ArrayListIterator pointing to the end of the ArrayList
 */
ArrayListIterator ArrayList_end(ArrayList* arrayList);

/**
 * Advances an ArrayListIterator
 * @param iterator The iterator to advance
 * @return The next item in the ArrayList, or null if no next item exists
 */
void* ArrayListIterator_next(ArrayListIterator* iterator);
/**
 * Regresses an ArrayListIterator
 * @param iterator The iterator to regress
 * @return The previous item in the ArrayList, or null if no previous item exists
 */
void* ArrayListIterator_previous(ArrayListIterator* iterator);

#endif //ENGINE_COLLECTIONS_ARRAYLIST_H