#include "linkedList.h"

#include <assert.h>
#include <stdlib.h>

#include "core/core.h"

static Node* createNode(const LinkedList* linkedList, const void* item);
static Node* getNode(const LinkedList* linkedList, size_t index);
static void removeNode(LinkedList* linkedList, Node* node);

LinkedList* LinkedList_create(const size_t itemSize) {
	// Return a nullptr if the item size is zero
	if (itemSize == 0) {
		return nullptr;
	}

	// Allocate memory for the LinkedList
	LinkedList* linkedList = Memory_malloc(sizeof(*linkedList));
	if (!linkedList) {
		return nullptr;
	}

	// Initialize the LinkedList
	linkedList->length = 0;
	linkedList->itemSize = itemSize;
	linkedList->head = nullptr;
	linkedList->tail = nullptr;

	return linkedList;
}

void LinkedList_destroy(LinkedList* linkedList) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return;
	}

	// Release the LinkedList and its nodes
	LinkedList_clear(linkedList);
	Memory_free(linkedList);
}

size_t LinkedList_getLength(const LinkedList* linkedList) {
	// Return 0 if the LinkedList is a nullptr
	if (!linkedList) {
		return 0;
	}

	// Return the length of the list
	return linkedList->length;
}

void LinkedList_clear(LinkedList* linkedList) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return;
	}

	// Free each node of the LinkedList
	Node* current = linkedList->head;
	while (current) {
		Node* next = current->next;
		Memory_free(current->data);
		Memory_free(current);
		current = next;
	}

	// Set the appropriate LinkedList state
	linkedList->head = nullptr;
	linkedList->tail = nullptr;
	linkedList->length = 0;
}

void* LinkedList_insert(LinkedList* linkedList, const void* item, const size_t index) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Return if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Push front the item if it should be inserted at the start of the list
	if (index == 0) {
		return LinkedList_pushFront(linkedList, item);
	}

	// Push back the item if it should be inserted at the end of the list
	if (index == linkedList->length) {
		return LinkedList_pushBack(linkedList, item);
	}

	// Return if the index is out of bounds
	if (index >= linkedList->length) {
		return nullptr;
	}

	// Get the next and previous nodes
	Node* next = getNode(linkedList, index);
	Node* previous = next->previous;

	// Create the node to insert
	Node* node = createNode(linkedList, item);
	if (!node) { return nullptr; }
	node->next = next;
	node->previous = previous;

	// Set the next and previous nodes to point to this node
	next->previous = node;
	previous->next = node;

	// Increment length
	linkedList->length++;

	// Return the inserted item
	return node->data;
}

void LinkedList_remove(LinkedList* linkedList, const size_t index) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return;
	}

	// Return if the index is out of bounds
	if (index <= linkedList->length) {
		return;
	}

	// Remove the node
	Node* node = getNode(linkedList, index);
	removeNode(linkedList, node);
}

void* LinkedList_getItem(LinkedList* linkedList, const size_t index) {
	// Return a nullptr if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Get the node
	Node* node = getNode(linkedList, index);
	if (!node) {
		return nullptr;
	}

	// Return the node data
	return node->data;
}

void* LinkedList_setItem(LinkedList* linkedList, const size_t index, const void* item) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Return if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Get the node to set the value of
	Node* node = getNode(linkedList, index);
	if (!node) {
		return nullptr;
	}

	// Copy the data to this node
	Memory_copy(node->data, item, linkedList->itemSize);

	// Return the node data
	return node->data;
}

void* LinkedList_pushFront(LinkedList* linkedList, const void* item) {
	// Return a nullptr if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Return a nullptr if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Create the node
	Node* node = createNode(linkedList, item);
	if (!node) {
		return nullptr;
	}

	// Set the next node to point to this node if it exists
	if (linkedList->head) {
		linkedList->head->previous = node;
	} else {
		linkedList->tail = node;
	}

	// Set this node as the head of the list
	linkedList->head = node;

	// Increment length
	linkedList->length++;

	// Return the node data
	return node->data;
}

void LinkedList_popFront(LinkedList* linkedList) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return;
	}

	// Return if the LinkedList has no items
	if (linkedList->length == 0) {
		return;
	}

	// Remove the first node
	Node* node = linkedList->head;
	removeNode(linkedList, node);
}

void* LinkedList_pushBack(LinkedList* linkedList, const void* item) {
	// Return a nullptr if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Return a nullptr if the item is a nullptr
	if (!item) {
		return nullptr;
	}

	// Create the node
	Node* node = createNode(linkedList, item);
	if (!node) { return nullptr; }
	node->previous = linkedList->tail;

	// Set the next node to point to this node if it exists
	if (linkedList->tail) {
		linkedList->tail->next = node;
	} else {
		linkedList->head = node;
	}

	// Set this node as the tail of the list
	linkedList->tail = node;

	// Increment length
	linkedList->length++;

	// Return the node data
	return node->data;
}

void LinkedList_popBack(LinkedList* linkedList) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return;
	}

	// Return if the LinkedList has no items
	if (linkedList->length == 0) {
		return;
	}

	// Remove the last node
	Node* node = linkedList->tail;
	removeNode(linkedList, node);
}

void* LinkedList_find(LinkedList* linkedList, const void* key, const LinkedListComparisonFn compare) {
	// Return if the LinkedList is a nullptr
	if (!linkedList) {
		return nullptr;
	}

	// Return if the key is a nullptr
	if (!key) {
		return nullptr;
	}

	// Return if the compare function is a nullptr
	if (!compare) {
		return nullptr;
	}

	// Iterate through the list
	const Node* current = linkedList->head;
	while (current) {
		if (compare(current->data, key) == 0) {
			return current->data;
		}
		current = current->next;
	}

	// Return nullptr if no comparison was successful
	return nullptr;
}

LinkedListIterator LinkedList_begin(LinkedList* linkedList) {
	return (LinkedListIterator) {
		.linkedList = linkedList,
		.current = linkedList ? linkedList->head : nullptr,
		.previous = nullptr
	};
}

bool LinkedListIterator_hasNext(LinkedListIterator* linkedListIterator) {
	// Return false if the LinkedListIterator is null
	if (!linkedListIterator) {
		return false;
	}

	// Return true if the current node is not null
	return linkedListIterator->current;
}

void* LinkedListIterator_next(LinkedListIterator* linkedListIterator) {
	// Return null if the iterator or current node is null
	if (!linkedListIterator || !linkedListIterator->current) {
		return nullptr;
	}

	// Set the current node to the previous and the next node to the current
	linkedListIterator->previous = linkedListIterator->current;
	linkedListIterator->current = linkedListIterator->current->next;

	// Return the previous data
	return linkedListIterator->previous->data;
}

void LinkedListIterator_remove(LinkedListIterator* linkedListIterator) {
	// Return if the iterator is null
	if (!linkedListIterator | !linkedListIterator->linkedList) {
		return;
	}

	// Return if the previous node is null
	if (!linkedListIterator->previous) {
		return;
	}

	// Remove the node from the linked list
	Node* node = linkedListIterator->previous;
	removeNode(linkedListIterator->linkedList, node);
}

static Node* createNode(const LinkedList* linkedList, const void* item) {
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
	Memory_copy(node->data, item, linkedList->itemSize);

	// Set the node's next and previous nodes to a nullptr
	node->next = nullptr;
	node->previous = nullptr;

	// Return the result
	return node;
}

static Node* getNode(const LinkedList* linkedList, const size_t index) {
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