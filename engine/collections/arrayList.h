#ifndef ENGINE_COLLECTIONS_ARRAYLIST_H
#define ENGINE_COLLECTIONS_ARRAYLIST_H

#include <stddef.h>

#include "collectionsCommon.h"

/**
 * A resizable array
 */
typedef struct arrayList {
	size_t length;
	size_t capacity;
	size_t itemSize;
	void* items;
} ArrayList;

/**
 * Allocates and initializes an ArrayList
 * @param itemSize The size of an item in bytes. Must be greater than zero.
 * @param capacity The initial capacity of the ArrayList
 * @return A pointer to the newly created ArrayList. The caller owns the returned object and must free it using ArrayList_destroy.
 */
ArrayList* ArrayList_create(size_t itemSize, size_t capacity);
/**
 * Destroys an ArrayList and releases all associated memory
 * @param arrayList The ArrayList to destroy
 */
void ArrayList_destroy(ArrayList* arrayList);

/**
 * Gets the length of an ArrayList
 * @param arrayList The ArrayList to get the length from
 * @return The length of the ArrayList
 */
size_t ArrayList_getLength(const ArrayList* arrayList);
/**
 * Checks whether an index is within the bounds of an ArrayList
 * @param arrayList The ArrayList to check
 * @param index The index to check
 * @return True if it is
 */
bool ArrayList_inBounds(const ArrayList* arrayList, size_t index);

/**
 * Removes all items from an ArrayList
 * @param arrayList The ArrayList to clear
 */
void ArrayList_clear(ArrayList* arrayList);
/**
 * Sets the capacity of an ArrayList.
 * @param arrayList The ArrayList to resize
 * @param capacity The desired capacity to resize to. Must be greater than its current length.
 */
void ArrayList_resize(ArrayList* arrayList, size_t capacity);

/**
 * Inserts a new item into an ArrayList at the specified index
 * @param arrayList The ArrayList to insert to
 * @param item The item to insert
 * @param index The index to insert the item at
 */
void ArrayList_insert(ArrayList* arrayList, const void* item, size_t index);
/**
 * Removes an item at the specified index from an ArrayList
 * @param arrayList The ArrayList to remove from
 * @param index The index of the item to remove
 */
void ArrayList_remove(ArrayList* arrayList, size_t index);

/**
 * Gets an item from the specified index of an ArrayList
 * @param arrayList The ArrayList to get the item from
 * @param index The index of the item to get
 * @return A pointer
 */
void* ArrayList_getItem(const ArrayList* arrayList, size_t index);
/**
 * Sets the value of an item at the specified index of an ArrayList
 * @param arrayList The ArrayList to index into
 * @param index The index to set
 * @param item The item to set. Must not be a nullptr.
 */
void ArrayList_setItem(const ArrayList* arrayList, size_t index, const void* item);

/**
 * Adds an item to the front of an ArrayList
 * @param arrayList The ArrayList to push to
 * @param item The item to push
 */
void ArrayList_pushFront(ArrayList* arrayList, const void* item);
/**
 * Removes the first item of an ArrayList
 * @param arrayList The ArrayList to pop from
 */
void ArrayList_popFront(ArrayList* arrayList);
/**
 * Adds an item to the end of an ArrayList
 * @param arrayList The ArrayList to push to
 * @param item The item to push
 */
void ArrayList_pushBack(ArrayList* arrayList, const void* item);
/**
 * Removes the last item of an ArrayList
 * @param arrayList The ArrayList to pop from
 */
void ArrayList_popBack(ArrayList* arrayList);


#endif //ENGINE_COLLECTIONS_ARRAYLIST_H
