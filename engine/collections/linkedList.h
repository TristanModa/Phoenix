#ifndef ENGINE_COLLECTIONS_LINKEDLIST_H
#define ENGINE_COLLECTIONS_LINKEDLIST_H

#include <stddef.h>

#include "common.h"

/**
 * An entry in a LinkedList
 */
typedef struct node {
	/**
	 * The item data
	 */
	void* data;
	/**
	 * The next node in the linked list, or null if this is the last node
	 */
	struct node* next;
	/**
	 * The previous node in the linked list, or null if this is the first node
	 */
	struct node* previous;
} Node;

/**
 * A non-contiguous doubly linked list.
 * All fields are managed by the set of LinkedList functions and should never be modified directly.
 */
typedef struct linkedList {
	/**
	 * The size of an item in bytes
	 */
	size_t itemSize;
	/**
	 * The number of items in the LinkedList
	 */
	size_t length;
	/**
	 * The first node in the LinkedList
	 */
	Node* head;
	/**
	 * The last node in the LinkedList
	 */
	Node* tail;
	/**
	 * The destructor to use when destroying LinkedList items
	 */
	CollectionsItemDestructorFn itemDestructor;
} LinkedList;

/**
 * An iterator for a LinkedList.
 * All fields are managed by the set of LinkedListIterator functions and should never be modified directly.
 */
typedef struct linkedListIterator {
	/**
	 * The associated LinkedList of the iterator
	 */
	LinkedList* linkedList;
	/**
	 * The current node of the iterator
	 */
	Node* next;
	/**
	 * The previous node of the iterator
	 */
	Node* previous;
} LinkedListIterator;

/**
 * Allocates and initializes a LinkedList
 * @param itemSize The size of an item in bytes. Must be greater than zero.
 * @param itemDestructor The destructor to use when destroying LinkedList items, or null for no destructor
 * @return A pointer to the newly created LinkedList. The caller owns the returned object and must free it using LinkedList_destroy().
 */
LinkedList* LinkedList_create(size_t itemSize, CollectionsItemDestructorFn itemDestructor);
/**
 * Destroys a LinkedList and releases all associated memory
 * @param linkedList The LinkedList to destroy
 */
void LinkedList_destroy(LinkedList* linkedList);

/**
 * Gets the length of a LinkedList
 * @param linkedList The LinkedList to get the length from
 * @return The length of the LinkedList
 */
size_t LinkedList_getLength(const LinkedList* linkedList);

/**
 * Destroys all LinkedList items and sets the length of the LinkedList to zero
 * @param linkedList The LinkedList to clear
 */
void LinkedList_clear(LinkedList* linkedList);

/**
 * Gets an item from the specified index of a LinkedList
 * @param linkedList The LinkedList to get the item from
 * @param index The index of the item to get
 * @return A pointer
 */
void* LinkedList_getItem(LinkedList* linkedList, size_t index);

/**
 * Inserts a new item into a LinkedList at the specified index.
 * The item data is shallow copied into the LinkedLists's internal storage.
 * Inserting an item invalidates all existing iterators.
 * @param linkedList The LinkedList to insert to
 * @param item The item to insert
 * @param index The index to insert the item at
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedList_insertItem(LinkedList* linkedList, const void* item, size_t index);
/**
 * Removes an item from the specified index of a LinkedList.
 * Removing an item invalidates all existing iterators.
 * @param linkedList The LinkedList to remove from
 * @param index The index to remove the item from
 * @return A pointer to the removed item, or null on failure.
 * The caller owns the returned object and is responsible for freeing it.
 */
void* LinkedList_removeItem(LinkedList* linkedList, size_t index);
/**
 * Destroys and removes an item from the specified index of a LinkedList.
 * Destroying an item invalidates all existing iterators.
 * @param linkedList The LinkedList to remove from
 * @param index The index to remove the item from
 */
void LinkedList_destroyItem(LinkedList* linkedList, size_t index);
/**
 * Replaces the item at the specified index of a LinkedList with a new item.
 * The item data is shallow copied into the LinkedList's internal storage.
 * @param linkedList The LinkedList of the item to replace
 * @param newItem The new item to replace the old item with
 * @param index The index of the item to replace
 * @return A pointer to the old item or null on failure.
 * The caller owns the returned object and is responsible for freeing it.
 */
void* LinkedList_replaceItem(LinkedList* linkedList, const void* newItem, size_t index);

/**
 * Executes an action on each item of a LinkedList
 * @param linkedList The LinkedList to iterate
 * @param action The action to execute
 */
void LinkedList_forEach(LinkedList* linkedList, CollectionsForEachActionFn action);
/**
 * Finds the first instance of an item that matches key in a LinkedList
 * @param linkedList The LinkedList to search
 * @param key The object to compare elements against
 * @param compare The function to use to compare the elements
 * @return The matching item, or nullptr on failure
 */
void* LinkedList_find(LinkedList* linkedList, const void* key, CollectionsCompareFn compare);

/**
 * Creates a LinkedListIterator pointing to the start of a LinkedList
 * @param linkedList The LinkedList to create the iterator with
 * @return A LinkedListIterator pointing to the start of the LinkedList
 */
LinkedListIterator LinkedList_begin(LinkedList* linkedList);
/**
 * Creates a LinkedListIterator pointing to the end of a LinkedList
 * @param linkedList The LinkedList to create the iterator with
 * @return A LinkedListIterator pointing to the end of the LinkedList
 */
LinkedListIterator LinkedList_end(LinkedList* linkedList);

/**
 * Advances a LinkedListIterator
 * @param iterator The iterator to advance
 * @return The next item in the LinkedList, or null if no next item exists
 */
void* LinkedListIterator_next(LinkedListIterator* iterator);
/**
 * Regresses a LinkedListIterator
 * @param iterator The iterator to regress
 * @return The previous item in the LinkedList, or null if no previous item exists
 */
void* LinkedListIterator_previous(LinkedListIterator* iterator);

#endif //ENGINE_COLLECTIONS_LINKEDLIST_H
