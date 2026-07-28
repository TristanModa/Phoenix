#ifndef ENGINE_COLLECTIONS_ARRAYLIST_H
#define ENGINE_COLLECTIONS_ARRAYLIST_H

#include <cc_array.h>

#include "collectionsCommon.h"

/**
 * Dynamic array that expands automatically as items are added.
 * Items are shallow copied into the ArrayList's internal storage.
 * Wrapper struct for CC_Array.
 */
typedef struct arrayList {
	/**
	 * The underlying CC_Array
	 */
	CC_Array* cc_array;
	/**
	 * The size of an item in bytes
	 */
	size_t itemSize;
	/**
	 * The destructor to call on item destruction
	 */
	ItemDestructorFn itemDestructor;
} ArrayList;

/**
 * Iterator for an ArrayList.
 * Wrapper for CC_ArrayIter.
 */
typedef struct arrayListIter {
	/**
	 * The underlying CC_ArrayIter
	 */
	CC_ArrayIter cc_arrayIter;
	/**
	 * The ArrayList of this iterator
	 */
	ArrayList* arrayList;
} ArrayListIter;

/**
 * Allocates and initializes an instance of an ArrayList
 * @param itemSize The size of an item in the ArrayList
 * @param itemDestructor The destructor for an ArrayList item. Pass null for no destructor.
 * @return The newly created ArrayList, or null on failure.
 * The caller owns the returned pointer and is responsible for freeing it using ArrayList_destroy()
 */
ArrayList* ArrayList_create(size_t itemSize, ItemDestructorFn itemDestructor);
/**
 * Destroys an ArrayList and releases all associated memory
 * @param arrayList The ArrayList to destroy
 */
void ArrayList_destroy(ArrayList* arrayList);

/**
 * Gets the number of items in an ArrayList
 * @param arrayList The ArrayList to get the size of
 * @return The size of the ArrayList, or SIZE_MAX on failure
 */
size_t ArrayList_getSize(const ArrayList* arrayList);

/**
 * Removes and destroys all items from an ArrayList
 * @param arrayList The ArrayList to clear
 */
void ArrayList_clear(ArrayList* arrayList);
/**
 * Resizes an ArrayList's internal storage so its capacity is equal to its size
 * @param arrayList The ArrayList to trim the capacity of
 */
void ArrayList_trimCapacity(ArrayList* arrayList);

/**
 * Gets an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to index into
 * @param index The index of the item
 * @return The item, or null on failure
 */
void* ArrayList_getItem(const ArrayList* arrayList, size_t index);
/**
 * Gets the item at the end of an ArrayList
 * @param arrayList The ArrayList to get the item from
 * @return The item, or null on failure
 */
void* ArrayList_getBackItem(const ArrayList* arrayList);

/**
 * Inserts an item at the specified index of an ArrayList.
 * The item is shallow copied before it is inserted, making it safe to insert temporary items.
 * @param arrayList The ArrayList to index into
 * @param index The index to insert to
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_insertItem(ArrayList* arrayList, size_t index, const void* item);
/**
 * Removes an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to index into
 * @param index The index to remove from
 * @param out A pointer to copy the removed item to. Pass null to destroy the item in place.
 */
void ArrayList_removeItem(ArrayList* arrayList, size_t index, void* out);
/**
 * Replaces an item at the specified index of an ArrayList with another.
 * The new item is shallow copied before it is inserted, making it safe to insert temporary items.
 * @param arrayList The ArrayList to index into
 * @param index The index to replace the item at
 * @param item The item to replace the old item with
 * @param out A pointer to copy the old item to. Pass null to destroy the item in place.
 * @return A pointer to the new item in the ArrayList, or null on failure
 */
void* ArrayList_replaceItem(ArrayList* arrayList, size_t index, const void* item, void* out);

/**
 * Inserts an item at the end of an ArrayList.
 * The item is shallow copied before it is inserted, making it safe to insert temporary items.
 * @param arrayList The ArrayList to insert into
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the ArrayList, or null on failure
 */
void* ArrayList_pushBackItem(ArrayList* arrayList, const void* item);
/**
 * Removes the item at the end of an ArrayList
 * @param arrayList The ArrayList to remove from
 * @param out A pointer to copy the removed item to. Pass null to destroy the item in place.
 */
void ArrayList_popBackItem(ArrayList* arrayList, void* out);

/**
 * Executes an action on each item in an ArrayList
 * @param arrayList The ArrayList to iterate through
 * @param action The action to execute
 */
void ArrayList_forEach(ArrayList* arrayList, ItemActionFn action);
/**
 * Gets the first occurance of an item in an ArrayList that matches the specified key
 * @param arrayList The ArrayList to search through
 * @param key The key to compare the item against
 * @param compare The function to use to compare items against the key
 * @return The first occurance of an item that matches the key, or null if the item was not found
 */
void* ArrayList_find(const ArrayList* arrayList, const void* key, ItemCompareFn compare);
/**
 * Sorts an ArrayList's items according to a comparison function
 * @param arrayList The ArrayList to sort the items of
 * @param compare The function to use to compare items against one another
 */
void ArrayList_sort(ArrayList* arrayList, ItemCompareFn compare);

/**
 * Initializes an ArrayListIter pointing to the beginning of an ArrayList
 * @param arrayList The ArrayList to iterate through
 * @param out A pointer to the ArrayListIter to initialize
 * @return True on success
 */
bool ArrayList_begin(ArrayList* arrayList, ArrayListIter* out);

/**
 * Gets the next item from and advances an ArrayListIter
 * @param it The ArrayListIter to get from and advance
 * @return The next item, null upon reaching the end of the list, or null on failure
 */
void* ArrayListIter_getNext(ArrayListIter* it);

/**
 * Gets the current index of an ArrayListIter
 * @param it The ArrayListIter to get the index of
 * @return The current index of the ArrayListIter, or SIZE_MAX on failure
 */
size_t ArrayListIter_getIndex(ArrayListIter* it);

/**
 * Inserts an item to an ArrayList during iteration without invalidating an ArrayListIter.
 * The item is shallow copied before it is inserted, making it safe to insert temporary items.
 * @param it The ArrayListIter to insert the item at
 * @param item The item to insert
 * @return The newly inserted item in the ArrayList, or null on failure
 */
void* ArrayListIter_insertItem(ArrayListIter* it, const void* item);
/**
 * Removes an item from an ArrayList during iteration without invalidating an ArrayListIter
 * @param it The ArrayListIter to remove the item at
 * @param out A pointer to copy the removed item to. Pass null to destroy the item in place.
 */
void ArrayListIter_removeItem(ArrayListIter* it, void* out);
/**
 * Replaces an item in an ArrayList with another during iteration without invalidating an ArrayListIter.
 * The replacement item is shallow copied before it is inserted, making it safe to insert temporary items.
 * @param it The ArrayListIter to replace the old item at
 * @param item The item to replace the old item with
 * @param out A pointer to copy the old item to. Pass null to destroy the item in place.
 * @return A pointer to the new item in the ArrayList, or null on failure
 */
void* ArrayListIter_replaceItem(ArrayListIter* it, const void* item, void* out);

#endif //ENGINE_COLLECTIONS_ARRAYLIST_H