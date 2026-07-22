#include "linkedList.h"

#include <assert.h>
#include <stdlib.h>

#include "core/core.h"

static Node* createNode(const LinkedList* linkedList, const void* item);
static Node* getNode(const LinkedList* linkedList, size_t index);
static void* insertNode(LinkedList* linkedList, size_t index, Node* node);
static void removeNode(LinkedList* linkedList, Node* node);

static Node* mergeSort(Node* head, CollectionsCompareFn compare);
static Node* mergeSortSplit(Node* head);
static Node* mergeSortMerge(Node* headA, Node* headB, CollectionsCompareFn compare);

LinkedList* LinkedList_create(const size_t itemSize, CollectionsItemDestructorFn itemDestructor) {
	// Return null if the item size is 0
	if (itemSize == 0) {
		Logger_error("Failed to create LinkedList: Item size cannot be 0");
		return nullptr;
	}

	// Allocate memory for the LinkedList instance
	LinkedList* linkedList = Memory_malloc(sizeof(*linkedList));
	if (!linkedList) {
		Logger_error("Failed to create LinkedList: Memory allocation failed for LinkedList instance");
		return nullptr;
	}

	// Initialize LinkedList fields
	linkedList->length = 0;
	linkedList->itemSize = itemSize;
	linkedList->head = nullptr;
	linkedList->tail = nullptr;
	linkedList->itemDestructor = itemDestructor;

	// Return the newly created LinkedList
	return linkedList;
}

bool LinkedList_destroy(LinkedList* linkedList) {
	// Return false if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to destroy LinkedList: LinkedList is null");
		return false;
	}

	// Clear the LinkedList
	LinkedList_clear(linkedList);

	// Release the LinkedList
	Memory_free(linkedList);

	// Return success
	return true;
}

size_t LinkedList_getLength(const LinkedList* linkedList) {
	// Return 0 if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to get length of LinkedList: LinkedList is null");
		return 0;
	}

	// Return the length of the list
	return linkedList->length;
}

bool LinkedList_clear(LinkedList* linkedList) {
	// Return false if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to clear LinkedList: LinkedList is null");
		return false;
	}

	// Free each node of the LinkedList
	Node* current = linkedList->head;
	while (current) {
		Node* next = current->next;
		if (linkedList->itemDestructor) {
			linkedList->itemDestructor(current->data);
		}
		Memory_free(current->data);
		Memory_free(current);
		current = next;
	}

	// Set the appropriate LinkedList state
	linkedList->head = nullptr;
	linkedList->tail = nullptr;
	linkedList->length = 0;

	// Return success
	return true;
}

void* LinkedList_getItem(LinkedList* linkedList, size_t index) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to get item from LinkedList: LinkedList is null");
		return nullptr;
	}

	// Return null if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to get item from LinkedList: Index %zu is out of bounds for LinkedList of length %zu",
			index, linkedList->length);
		return nullptr;
	}

	// Return the node data
	const Node* node = getNode(linkedList, index);
	return node->data;
}

void* LinkedList_insertItem(LinkedList* linkedList, const size_t index, const void* item) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to insert item to LinkedList: LinkedList is null");
		return nullptr;
	}

	// Return if the item is null
	if (!item) {
		Logger_error("Failed to insert item to LinkedList: Item is null");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index > linkedList->length) {
		Logger_error(
			"Failed to insert item to LinkedList: Index %zu is out of bounds for LinkedList of length %zu",
			index, linkedList->length);
		return nullptr;
	}

	// Create the node to insert
	Node* node = createNode(linkedList, item);
	if (!node) {
		Logger_error("Failed to insert item to LinkedList: Memory allocation for LinkedList node failed");
		return nullptr;
	}

	// Insert the node
	insertNode(linkedList, index, node);

	// Return the node data
	return node->data;
}

bool LinkedList_removeItem(LinkedList* linkedList, size_t index, void* out) {
	// Return failure if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to remove item from LinkedList: LinkedList is null");
		return false;
	}
	
	// Return failure if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to remove item from LinkedList: Index %zu is out of bounds for LinkedList of length %zu",
			index, linkedList->length);
		return false;
	}
	
	// Get the node at the index
	Node* node = getNode(linkedList, index);

	// Copy the item to out, or destroy it if out is null
	if (out) memcpy(out, node->data, linkedList->itemSize);
	else if (linkedList->itemDestructor) linkedList->itemDestructor(node->data);

	// Remove the node from the LinkedList
	removeNode(linkedList, node);

	// Return success
	return true;
}

void* LinkedList_replaceItem(LinkedList* linkedList, size_t index, const void* newItem, void* out) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to replace LinkedList item: LinkedList is null");
		return nullptr;
	}

	// Return null if the new item is null
	if (!newItem) {
		Logger_error("Failed to replace LinkedList item: Item is null");
		return nullptr;
	}

	// Return null if the index is out of bounds
	if (index > linkedList->length) {
		Logger_error(
			"Failed to replace LinkedList item: Index %zu is out of bounds for LinkedList of length %zu",
			index, linkedList->length);
		return nullptr;
	}

	// Get the node to replace
	Node* node = getNode(linkedList, index);

	// Copy the item to out, or destroy it if out is null
	void* item = node->data;
	if (out) memcpy(out, item, linkedList->itemSize);
	else if (linkedList->itemDestructor) linkedList->itemDestructor(item);

	// Set the new item
	memcpy(node->data, newItem, linkedList->itemSize);

	// Return the new item
	return node->data;
}

void* LinkedList_pushBackItem(LinkedList* linkedList, const void* item) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to push back item to LinkedList: LinkedList is null");
		return nullptr;
	}

	// Return null if the item is null
	if (!item) {
		Logger_error("Failed to push back item to LinkedList: Item is null");
		return nullptr;
	}

	// Create a node for this item
	Node* node = createNode(linkedList, item);
	if (!node) {
		Logger_error("Failed to push back item to LinkedList: Memory allocation for LinkedList node failed");
		return nullptr;
	}

	// Insert the node
	return insertNode(linkedList, linkedList->length, node);
}

bool LinkedList_popBackItem(LinkedList* linkedList, void* out) {
	// Return failure if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to pop back item from LinkedList: LinkedList is null");
		return false;
	}

	// Return failure if the LinkedList is empty
	if (linkedList->length == 0) {
		Logger_error("Failed to pop back item from LinkedList: LinkedList is empty");
		return false;
	}

	// Get the back node
	Node* node = getNode(linkedList, linkedList->length - 1);

	// Copy the item to out, or destroy it if out is null
	if (out) memcpy(out, node->data, linkedList->itemSize);
	else if (linkedList->itemDestructor) linkedList->itemDestructor(node->data);

	// Remove the node from the LinkedList
	removeNode(linkedList, node);

	// Return success
	return true;
}

bool LinkedList_forEach(LinkedList* linkedList, CollectionsForEachActionFn action) {
	// Return failure if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to execute forEach on LinkedList: LinkedList is null");
		return false;
	}

	// Return failure if the action is null
	if (!action) {
		Logger_error("Failed to execute forEach on LinkedList: Action is null");
		return false;
	}

	// Iterate through the LinkedList and execute the action on each item
	LinkedListIterator it = LinkedList_begin(linkedList);
	void* item;
	while ((item = LinkedListIterator_next(&it))) {
		action(item);
	}

	// Return success
	return true;
}

void* LinkedList_find(LinkedList* linkedList, const void* key, CollectionsCompareFn compare) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed find LinkedList item: LinkedList is null");
		return nullptr;
	}

	// Return if the key is null
	if (!key) {
		Logger_error("Failed find LinkedList item: Key is null");
		return nullptr;
	}

	// Return if the compare function is null
	if (!compare) {
		Logger_error("Failed find LinkedList item: Compare function is null");
		return nullptr;
	}

	// Iterate through the LinkedList until a match is found
	void* result = nullptr;
	void* item;
	LinkedListIterator it = LinkedList_begin(linkedList);
	while ((item = LinkedListIterator_next(&it))) {
		if (compare(item, key) == 0) {
			result = item;
			break;
		}
	}

	// Return the result
	return result;
}

bool LinkedList_sort(LinkedList* linkedList, CollectionsCompareFn compare) {
	// Return failure if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to sort LinkedList: LinkedList is null");
		return false;
	}

	// Return failure if the action is null
	if (!compare) {
		Logger_error("Failed to sort LinkedList: Action is null");
		return false;
	}

	// Sort the linked list by the comparison function
	linkedList->head = mergeSort(linkedList->head, compare);

	// Return success
	return true;
}

LinkedListIterator LinkedList_begin(LinkedList* linkedList) {
	// Return an empty iterator if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to create LinkedList iterator: LinkedList is null");
		return (LinkedListIterator){};
	}

	// Return an iterator starting at the beginning of the LinkedList
	return (LinkedListIterator){
		.linkedList = linkedList,
		.next = linkedList->head,
		.previous = nullptr,
		.lastReturned = nullptr
	};
}

LinkedListIterator LinkedList_end(LinkedList* linkedList) {
	// Return an empty iterator if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to create LinkedList iterator: LinkedList is null");
		return (LinkedListIterator){};
	}

	// Return an iterator starting at the end of the LinkedList
	return (LinkedListIterator){
		.linkedList = linkedList,
		.next = nullptr,
		.previous = linkedList->tail,
		.lastReturned = nullptr
	};
}

void* LinkedListIterator_next(LinkedListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to advance LinkedListIterator: LinkedListIterator is null");
		return nullptr;
	}

	// Return null if the next node is null
	if (!iterator->next) {
		return nullptr;
	}

	// Get the next node
	Node* node = iterator->next;

	// Update the iterator state
	iterator->previous = node;
	iterator->next = node->next;
	iterator->lastReturned = node;

	// Return the node
	return node->data;
}

void* LinkedListIterator_previous(LinkedListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to regress LinkedListIterator: LinkedListIterator is null");
		return nullptr;
	}

	// Return null if the previous node is null
	if (!iterator->previous) {
		return nullptr;
	}

	// Get the previous node
	Node* node = iterator->previous;

	// Update the iterator state
	iterator->next = node;
	iterator->previous = node->previous;
	iterator->lastReturned = node;

	// Return the node
	return node->data;
}

void* LinkedListIterator_insertItem(LinkedListIterator* iterator, void* item) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform LinkedList item insertion with LinkedListIterator: "
			"LinkedListIterator is null");
		return nullptr;
	}

	// Return null if the item is null
	if (!item) {
		Logger_error(
			"Failed to perform LinkedList item insertion with LinkedListIterator: "
			"Item is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->linkedList) {
		Logger_error(
			"Failed to perform LinkedList item insertion with LinkedListIterator: "
			"LinkedListIterator is invalid");
		return nullptr;
	}

	// Create the node to insert
	Node* node = createNode(iterator->linkedList, item);
	if (!node) {
		Logger_error(
			"Failed to perform LinkedList item insertion with LinkedListIterator: "
			"Memory allocation for LinkedList node failed");
		return nullptr;
	}

	// Set this node's next and previous nodes
	node->next = iterator->next;
	node->previous = iterator->previous;

	// Update the node's previous node to point to it
	if (node->next) {
		node->next->previous = node;
	} else {
		iterator->linkedList->tail = node;
	}

	// Update the node's previous node to point to it
	if (node->previous) {
		node->previous->next = node;
	} else {
		iterator->linkedList->head = node;
	}

	// Increment length
	iterator->linkedList->length++;

	// Update the iterator
	iterator->previous = node;

	// Return the node data
	return node->data;
}

bool LinkedListIterator_removeItem(LinkedListIterator* iterator, void* out) {
	// Return failure if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform LinkedList item removal with LinkedListIterator: "
			"LinkedListIterator is null");
		return false;
	}

	// Return failure if the iterator is in an invalid state
	if (!iterator->linkedList) {
		Logger_error(
			"Failed to perform LinkedList item removal with LinkedListIterator: "
			"LinkedListIterator is invalid");
		return false;
	}

	// Return failure if there is no last returned element to remove
	if (!iterator->lastReturned) {
		Logger_error(
			"Failed to perform LinkedList item removal with LinkedListIterator: "
			"No item has been traversed yet, or the item has already been removed");
		return false;
	}

	// Get the node to remove
	Node* node = iterator->lastReturned;

	// Copy the node's item to out, or destroy it if out is null
	if (out) {
		memcpy(out, node->data, iterator->linkedList->itemSize);
	} else if (iterator->linkedList->itemDestructor) {
		iterator->linkedList->itemDestructor(node->data);
	}

	// Fix iterator state
	iterator->lastReturned = nullptr;
	if (node == iterator->next) iterator->next = node->next;
	if (node == iterator->previous) iterator->previous = node->previous;

	// Remove the node from the LinkedList
	removeNode(iterator->linkedList, node);

	// Return success
	return true;
}

void* LinkedListIterator_replaceItem(LinkedListIterator* iterator, const void* newItem, void* out) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error(
			"Failed to perform LinkedList item replacement with LinkedListIterator: "
			"LinkedListIterator is null");
		return nullptr;
	}

	// Return null if the iterator is in an invalid state
	if (!iterator->linkedList) {
		Logger_error(
			"Failed to perform LinkedList item replacement with LinkedListIterator: "
			"LinkedListIterator is invalid");
		return nullptr;
	}

	// Return null if there is no last returned element to remove
	if (!iterator->lastReturned) {
		Logger_error(
			"Failed to perform LinkedList item replacement with LinkedListIterator: "
			"No item has been traversed yet, or the item has already been removed");
		return nullptr;
	}

	// Get the node to replace
	Node* node = iterator->lastReturned;

	// Copy the item to out, or destroy it if out is null
	void* item = node->data;
	if (out) memcpy(out, item, iterator->linkedList->itemSize);
	else if (iterator->linkedList->itemDestructor) iterator->linkedList->itemDestructor(item);

	// Set the new item
	memcpy(node->data, newItem, iterator->linkedList->itemSize);

	// Return the new item
	return node->data;
}

Node* createNode(const LinkedList* linkedList, const void* item) {
	assert(linkedList);
	assert(item);

	// Allocate memory for the node
	Node* node = Memory_malloc(sizeof(*node));
	if (!node) {
		return nullptr;
	}

	// Allocate memory for the node data
	node->data = Memory_malloc(linkedList->itemSize);
	if (!node->data) {
		Memory_free(node);
		return nullptr;
	}

	// Copy the item into the node data
	memcpy(node->data, item, linkedList->itemSize);

	// Set the node's next and previous nodes to null
	node->next = nullptr;
	node->previous = nullptr;

	// Return the result
	return node;
}

Node* getNode(const LinkedList* linkedList, const size_t index) {
	assert(linkedList);
	assert(index < linkedList->length);

	// Iterate through the list until index is reached
	Node* current;
	if (index < linkedList->length / 2) {
		current = linkedList->head;
		for (size_t i = 0; i < index; i++) {
			current = current->next;
		}
	} else {
		current = linkedList->tail;
		for (size_t i = linkedList->length - 1; i > index; i--) {
			current = current->previous;
		}
	}

	// Return the result
	return current;
}

void* insertNode(LinkedList* linkedList, size_t index, Node* node) {
	assert(linkedList);
	assert(index <= linkedList->length);
	assert(node);

	// Get the next and previous nodes
	Node* next;
	Node* previous;
	if (linkedList->length == 0) {
		next = nullptr;
		previous = nullptr;
	} else if (index != linkedList->length) {
		next = getNode(linkedList, index);
		previous = next->previous;
	} else {
		previous = getNode(linkedList, index - 1);
		next = previous->next;
	}

	// Set this node's next and previous nodes
	node->next = next;
	node->previous = previous;

	// Set the next node to point to this node
	if (node->next) {
		node->next->previous = node;
	} else {
		linkedList->tail = node;
	}

	// Set the previous node to point to this node
	if (node->previous) {
		node->previous->next = node;
	} else {
		linkedList->head = node;
	}

	// Increment length
	linkedList->length++;

	// Return the inserted item
	return node->data;
}

void removeNode(LinkedList* linkedList, Node* node) {
	assert(linkedList);
	assert(node);

	// Set the previous node to point to the next node
	if (linkedList->head != node) {
		node->previous->next = node->next;
	} else {
		linkedList->head = node->next;
	}

	// Set the next node to point to the previous
	if (linkedList->tail != node) {
		node->next->previous = node->previous;
	} else {
		linkedList->tail = node->previous;
	}

	// Release the node and its data
	Memory_free(node->data);
	Memory_free(node);

	// Decrement length
	linkedList->length--;
}

Node* mergeSort(Node* head, CollectionsCompareFn compare) {
	assert(compare);

	// Return if the LinkedList is empty or has only one node
	if (!head || !head->next) {
		return head;
	}

	// Split the LinkedList in half
	Node* headA = head;
	Node* headB = mergeSortSplit(head);

	// Recursively sort each half
	headA = mergeSort(headA, compare);
	headB = mergeSort(headB, compare);

	// Merge the two halves
	return mergeSortMerge(headA, headB, compare);
}

Node* mergeSortSplit(Node* head) {
	// Create pointers fast and slow for traversing the LinkedList
	Node* fast = head;
	Node* slow = head;

	// Traverse the list until fast reaches the end
	while (fast && fast->next && fast->next->next) {
		fast = fast->next->next;
		slow = slow->next;
	}

	// Split the list at the middle node
	Node* middle = slow->next;
	slow->next = nullptr;
	if (middle) middle->previous = nullptr;

	// Return the middle node
	return middle;
}

Node* mergeSortMerge(Node* headA, Node* headB, CollectionsCompareFn compare) {
	assert(compare);

	// If either list is empty, return the other list
	if (!headA) return headB;
	if (!headB) return headA;

	// Determine the head of the merged list
	Node* result;
	if (compare(headA->data, headB->data) < 0) {
		result = headA;
		headA = headA->next;
	} else {
		result = headB;
		headB = headB->next;
	}

	// Loop until one of the sublists runs out of nodes
	Node* tail = result;
	while (headA && headB) {
		if (compare(headA->data, headB->data) < 0) {
			tail->next = headA;
			headA->previous = tail;
			headA = headA->next;
		} else {
			tail->next = headB;
			headB->previous = tail;
			headB = headB->next;
		}
		tail = tail->next;
	}

	// Attach any remaining if one of the lists is not empty
	if (headA) {
		tail->next = headA;
		headA->previous = tail;
	} else if (headB) {
		tail->next = headB;
		headB->previous = tail;
	}

	// Return result
	return result;
}