#include "linkedList.h"

#include <stdlib.h>

#include "collectionsCommon.h"
#include "core/core.h"

static Node* createNode(const void* item, size_t itemSize);
static Node* getNode(const LinkedList* linkedList, size_t index);

LinkedList* LinkedList_create(const size_t itemSize) {
	// Throw a fatal error if the item size is zero
	if (itemSize == 0) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_CREATE COLLECTIONS_ERR_ITEM_SIZE_ZERO,
			COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Allocate memory for the linked list
	LinkedList* linkedList = Memory_malloc(sizeof(LinkedList));
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_CREATE COLLECTIONS_ERR_MALLOC_FAIL,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Initialize the linked list
	linkedList->length = 0;
	linkedList->itemSize = itemSize;
	linkedList->head = nullptr;
	linkedList->tail = nullptr;
	return linkedList;
}

void LinkedList_destroy(LinkedList* linkedList) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_DESTROY COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Release the LinkedList and its nodes
	LinkedList_clear(linkedList);
	Memory_free(linkedList);
}

size_t LinkedList_getLength(const LinkedList* linkedList) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_error(
			COLLECTIONS_ERR_FAILED_GET_LENGTH COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	return linkedList->length;
}

bool LinkedList_inBounds(const LinkedList* linkedList, const size_t index) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_BOUNDS_CHECK COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	return index < linkedList->length;
}

void LinkedList_clear(LinkedList* linkedList) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_CLEAR COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
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

void LinkedList_insert(LinkedList* linkedList, const void* item, const size_t index) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Item");
		exit(EXIT_FAILURE);
	}

	// Push front the element if it should be inserted at the start of the list
	if (index == 0) {
		LinkedList_pushFront(linkedList, item);
		return;
	}

	// Push back the item if it should be inserted at the end of the list
	if (index == linkedList->length) {
		LinkedList_pushBack(linkedList, item);
		return;
	}

	// Throw a fatal error if the index is out of bounds
	if (!LinkedList_inBounds(linkedList, index)) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_INDEX_OOB,
			COLLECTIONS_ERR_LINKED_LIST, index, COLLECTIONS_ERR_LINKED_LIST, linkedList->length);
		exit(EXIT_FAILURE);
	}

	// Get the next and previous nodes
	Node* next = getNode(linkedList, index);
	Node* previous = next->previous;

	// Create the node to insert
	Node* node = createNode(item, linkedList->itemSize);
	if (!node) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_INSERT COLLECTIONS_ERR_MALLOC_FAIL,
			COLLECTIONS_ERR_LINKED_LIST, "node");
		exit(EXIT_FAILURE);
	}
	node->next = next;
	node->previous = previous;

	// Set the next and previous nodes to point to this node
	next->previous = node;
	previous->next = node;

	// Increment length
	linkedList->length++;
}

void LinkedList_remove(LinkedList* linkedList, const size_t index) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_REMOVE COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the index is outside the bounds of the LinkedList
	if (!LinkedList_inBounds(linkedList, index)) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_REMOVE COLLECTIONS_ERR_INDEX_OOB,
			COLLECTIONS_ERR_LINKED_LIST, index, COLLECTIONS_ERR_LINKED_LIST, linkedList->length);
		exit(EXIT_FAILURE);
	}

	// Pop front the element if it should be removed from the start of the list
	if (index == 0) {
		LinkedList_popFront(linkedList);
		return;
	}

	// Pop back the element if it should be removed from the end of the list
	if (index == linkedList->length - 1) {
		LinkedList_popBack(linkedList);
		return;
	}

	// Get the node to remove
	Node* node = getNode(linkedList, index);

	// Point the next and previous nodes to each other
	node->next->previous = node->previous;
	node->previous->next = node->next;

	// Release the node and its data
	Memory_free(node->data);
	Memory_free(node);

	// Decrement the length
	linkedList->length--;
}

void* LinkedList_getItem(const LinkedList* linkedList, const size_t index) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_GET COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Get the node
	Node* node = getNode(linkedList, index);
	if (!node) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_GET COLLECTIONS_ERR_INDEX_OOB,
			COLLECTIONS_ERR_LINKED_LIST, index, COLLECTIONS_ERR_LINKED_LIST, linkedList->length);
		exit(EXIT_FAILURE);
	}

	// Return the node data
	return node->data;
}

void LinkedList_setItem(LinkedList* linkedList, const size_t index, const void* item) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Item");
		exit(EXIT_FAILURE);
	}

	// Get the node to set the value of
	Node* node = getNode(linkedList, index);
	if (!node) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_SET COLLECTIONS_ERR_INDEX_OOB,
			COLLECTIONS_ERR_LINKED_LIST, index, COLLECTIONS_ERR_LINKED_LIST, linkedList->length);
		exit(EXIT_FAILURE);
	}

	// Copy the data to this node
	memcpy(node->data, item, linkedList->itemSize);
}

void LinkedList_pushFront(LinkedList* linkedList, const void* item) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Item");
		exit(EXIT_FAILURE);
	}

	// Create the node
	Node* node = createNode(item, linkedList->itemSize);
	if (!node) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_MALLOC_FAIL,
			COLLECTIONS_ERR_LINKED_LIST, "node");
		exit(EXIT_FAILURE);
	}
	node->next = linkedList->head;

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
}

void LinkedList_popFront(LinkedList* linkedList) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the LinkedList is a nullptr
	if (linkedList->length == 0) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_POP_EMPTY,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Set the next node to be the head
	Node* node = linkedList->head;
	linkedList->head = node->next;

	// Remove references to the removed node
	if (linkedList->head) {
		linkedList->head->previous = nullptr;
	} else {
		linkedList->tail = nullptr;
	}

	// Release the node and its data
	Memory_free(node->data);
	Memory_free(node);

	// Decrement length
	linkedList->length--;
}

void LinkedList_pushBack(LinkedList* linkedList, const void* item) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the item is a nullptr
	if (!item) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Item");
		exit(EXIT_FAILURE);
	}

	// Create the node
	Node* node = createNode(item, linkedList->itemSize);
	if (!node) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_PUSH COLLECTIONS_ERR_MALLOC_FAIL,
			COLLECTIONS_ERR_LINKED_LIST, "node");
		exit(EXIT_FAILURE);
	}
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
}

void LinkedList_popBack(LinkedList* linkedList) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the LinkedList is a nullptr
	if (linkedList->length == 0) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_POP COLLECTIONS_ERR_POP_EMPTY,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Set the next node to be the head
	Node* node = linkedList->tail;
	linkedList->tail = node->previous;

	// Remove references to the removed node
	if (linkedList->tail) {
		linkedList->tail->next = nullptr;
	} else {
		linkedList->head = nullptr;
	}

	// Release the node and its data
	Memory_free(node->data);
	Memory_free(node);

	// Decrement length
	linkedList->length--;
}

void* LinkedList_find(const LinkedList* linkedList, const void* key, const CollectionsComparisonFn compare) {
	// Throw a fatal error if the LinkedList is a nullptr
	if (!linkedList) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, COLLECTIONS_ERR_LINKED_LIST);
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the key is a nullptr
	if (!key) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Item");
		exit(EXIT_FAILURE);
	}

	// Throw a fatal error if the compare function is a nullptr
	if (!compare) {
		Logger_fatal(
			COLLECTIONS_ERR_FAILED_FIND COLLECTIONS_ERR_NULLPTR,
			COLLECTIONS_ERR_LINKED_LIST, "Compare function");
		exit(EXIT_FAILURE);
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

Node* createNode(const void* item, const size_t itemSize) {
	// Allocate memory for the node
	Node* node = Memory_malloc(sizeof(Node));
	if (!node) {
		return nullptr;
	}

	// Allocate memory for the node data
	node->data = Memory_malloc(itemSize);
	if (!node->data) {
		Memory_free(node);
		return nullptr;
	}

	// Copy the item into the node data
	memcpy(node->data, item, itemSize);

	// Set the node's next and previous nodes to a nullptr
	node->next = nullptr;
	node->previous = nullptr;

	// Return the result
	return node;
}

Node* getNode(const LinkedList* linkedList, const size_t index) {
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
