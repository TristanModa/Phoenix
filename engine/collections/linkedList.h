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
	 * The next node in the linked list
	 */
	Node* next;
	/**
	 * The previous node in the linked list
	 */
	Node* previous;
	/**
	 * The last node the iterator returned
	 */
	Node* lastReturned;
} LinkedListIterator;

/**
 * Initializes a LinkedList
 * @param itemSize The size of an item in bytes. Must be greater than 0.
 * @param itemDestructor The destructor to use when destroying LinkedList items, or null for no destructor
 * @return A pointer to the newly created LinkedList.
 * The caller owns the returned object and must free it using LinkedList_destroy().
 */
LinkedList* LinkedList_create(size_t itemSize, CollectionsItemDestructorFn itemDestructor);
/**
 * Destroys a LinkedList and releases all associated memory
 * @param linkedList The LinkedList to destroy
 * @return True on success
 */
bool LinkedList_destroy(LinkedList* linkedList);

/**
 * Gets the length of a LinkedList
 * @param linkedList The LinkedList to get the length of
 * @return The length of the LinkedList, or 0 on failure
 */
size_t LinkedList_getLength(const LinkedList* linkedList);

/**
 * Destroys all items in a LinkedList
 * @param linkedList The LinkedList to clear
 * @return True on success
 */
bool LinkedList_clear(LinkedList* linkedList);

/**
 * Gets an item from the specified index of a LinkedList
 * @param linkedList The LinkedList to get the item from
 * @param index The index of the item to get
 * @return A pointer to the item, or null on failure
 */
void* LinkedList_getItem(LinkedList* linkedList, size_t index);

/**
 * Inserts a new item to a LinkedList at the specified index.
 * The item data is shallow copied into the LinkedLists's internal storage.
 * @param linkedList The LinkedList to insert to
 * @param index The index to insert the item at
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedList_insertItem(LinkedList* linkedList, size_t index, const void* item);
/**
 * Removes an item from the specified index of a LinkedList
 * @param linkedList The LinkedList to remove from
 * @param index The index to remove the item from
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool LinkedList_removeItem(LinkedList* linkedList, size_t index, void* out);
/**
 * Replaces an item at the specified index of an LinkedList with another.
 * The item data is shallow copied into the LinkedList's internal storage.
 * @param linkedList The LinkedList containing the item to replace
 * @param index The index of the item to replace
 * @param newItem The item to insert
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedList_replaceItem(LinkedList* linkedList, size_t index, const void* newItem, void* out);

/**
 * Inserts a new item to the end of an LinkedList.
 * The item data is shallow copied into the LinkedList's internal storage.
 * @param linkedList The LinkedList to insert to
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedList_pushBackItem(LinkedList* linkedList, const void* item);
/**
 * Removes the item at the end of a LinkedList
 * @param linkedList The LinkedList to remove from
 * @param out A pointer to the location to store the removed item.
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool LinkedList_popBackItem(LinkedList* linkedList, void* out);

/**
 * Executes an action on each item of a LinkedList
 * @param linkedList The LinkedList to iterate
 * @param action The action to execute
 * @return True on success
 */
bool LinkedList_forEach(LinkedList* linkedList, CollectionsForEachActionFn action);
/**
 * Finds the first instance of an item that matches the specified key in a LinkedList
 * @param linkedList The LinkedList to search
 * @param key The object to compare items against
 * @param compare The function to use to compare the items
 * @return The matching item, or null on failure
 */
void* LinkedList_find(LinkedList* linkedList, const void* key, CollectionsCompareFn compare);
/**
 * Sorts a LinkedList according to a comparison function
 * @param linkedList The LinkedList to sort
 * @param compare The comparison function to use
 * @return True on success
 */
bool LinkedList_sort(LinkedList* linkedList, CollectionsCompareFn compare);

/**
 * Creates a LinkedListIterator pointing to the start of a LinkedList
 * @param linkedList The LinkedList to create the LinkedListIterator with
 * @return A LinkedListIterator pointing to the start of the LinkedList
 */
LinkedListIterator LinkedList_begin(LinkedList* linkedList);
/**
 * Creates a LinkedListIterator pointing to the end of a LinkedList
 * @param linkedList The LinkedList to create the LinkedListIterator with
 * @return A LinkedListIterator pointing to the end of the LinkedList
 */
LinkedListIterator LinkedList_end(LinkedList* linkedList);

/**
 * Advances a LinkedListIterator
 * @param iterator The LinkedListIterator to advance
 * @return The next item in the LinkedList, or null if no next item exists
 */
void* LinkedListIterator_next(LinkedListIterator* iterator);
/**
 * Regresses a LinkedListIterator
 * @param iterator The LinkedListIterator to regress
 * @return The previous item in the LinkedList, or null if no previous item exists
 */
void* LinkedListIterator_previous(LinkedListIterator* iterator);

/**
 * Inserts a new item to a LinkedList at the position of a LinkedListIterator.
 * The item data is shallow copied into the LinkedList's internal storage.
 * @param iterator The LinkedListIterator to insert at
 * @param item The item to insert
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedListIterator_insertItem(LinkedListIterator* iterator, void* item);
/**
 * Removes an item from a LinkedList at the position of a LinkedListIterator.
 * @param iterator The iterator to remove at
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return True on success
 */
bool LinkedListIterator_removeItem(LinkedListIterator* iterator, void* out);
/**
 * Replaces an LinkedList item at position of an LinkedListIterator with another.
 * The item data is shallow copied into the LinkedList's internal storage.
 * @param iterator The LinkedList containing the item to replace
 * @param newItem The item to insert
 * @param out A pointer to the location to store the removed item
 * Pass null to destroy the item in place.
 * @return A pointer to the newly inserted item in the LinkedList, or null on failure
 */
void* LinkedListIterator_replaceItem(LinkedListIterator* iterator, const void* newItem, void* out);

#endif //ENGINE_COLLECTIONS_LINKEDLIST_H
