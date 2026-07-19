#ifndef ENGINE_COLLECTIONS_LINKEDLIST_H
#define ENGINE_COLLECTIONS_LINKEDLIST_H

#include <stddef.h>

#include "collectionsCommon.h"

/**
 * An entry in a LinkedList
 */
typedef struct node {
	void* data;
	struct node* next;
	struct node* previous;
} Node;

/**
 *
 */
typedef struct linkedList {
	size_t length;
	size_t itemSize;
	Node* head;
	Node* tail;
} LinkedList;

/**
 * Allocates and initializes an LinkedList
 * @param itemSize The size of an item in bytes. Must be greater than zero.
 * @return A pointer to the newly created LinkedList. The caller owns the returned object and must free it using LinkedList_destroy.
 */
LinkedList* LinkedList_create(size_t itemSize);
/**
 * Destroys an LinkedList and releases all associated memory
 * @param linkedList The LinkedList to destroy
 */
void LinkedList_destroy(LinkedList* linkedList);

/**
 * Gets the length of an LinkedList
 * @param linkedList The LinkedList to get the length from
 * @return The length of the LinkedList
 */
size_t LinkedList_getLength(const LinkedList* linkedList);
/**
 * Checks whether an index is within the bounds of an LinkedList
 * @param linkedList The LinkedList to check
 * @param index The index to check
 * @return True if it is
 */
bool LinkedList_inBounds(const LinkedList* linkedList, size_t index);

/**
 * Removes all items from an LinkedList
 * @param linkedList The LinkedList to clear
 */
void LinkedList_clear(LinkedList* linkedList);

/**
 * Inserts a new item into an LinkedList at the specified index
 * @param linkedList The LinkedList to insert to
 * @param item The item to insert
 * @param index The index to insert the item at
 */
void LinkedList_insert(LinkedList* linkedList, const void* item, size_t index);
/**
 * Removes an item at the specified index from an LinkedList
 * @param linkedList The LinkedList to remove from
 * @param index The index of the item to remove
 */
void LinkedList_remove(LinkedList* linkedList, size_t index);

/**
 * Gets an item from the specified index of an LinkedList
 * @param linkedList The LinkedList to get the item from
 * @param index The index of the item to get
 * @return A pointer
 */
void* LinkedList_getItem(const LinkedList* linkedList, size_t index);
/**
 * Sets the value of an item at the specified index of an LinkedList
 * @param linkedList The LinkedList to index into
 * @param index The index to set
 * @param item The item to set. Must not be a nullptr.
 */
void LinkedList_setItem(LinkedList* linkedList, size_t index, const void* item);

/**
 * Adds an item to the front of an LinkedList
 * @param linkedList The LinkedList to push to
 * @param item The item to push
 */
void LinkedList_pushFront(LinkedList* linkedList, const void* item);
/**
 * Removes the first item of an LinkedList
 * @param linkedList The LinkedList to pop from
 */
void LinkedList_popFront(LinkedList* linkedList);
/**
 * Adds an item to the end of an LinkedList
 * @param linkedList The LinkedList to push to
 * @param item The item to push
 */
void LinkedList_pushBack(LinkedList* linkedList, const void* item);
/**
 * Removes the last item of an LinkedList
 * @param linkedList The LinkedList to pop from
 */
void LinkedList_popBack(LinkedList* linkedList);

void* LinkedList_find(const LinkedList* linkedList, const void* key, CollectionsComparisonFn compare);

#endif //ENGINE_COLLECTIONS_LINKEDLIST_H
