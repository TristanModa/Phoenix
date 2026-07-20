#include "linkedList.h"

#include <assert.h>
#include <stdlib.h>

#include "core/core.h"

static Node* createNode(const LinkedList* linkedList, const void* item);
static Node* getNode(const LinkedList* linkedList, size_t index);
static void removeNode(LinkedList* linkedList, Node* node);

LinkedList* LinkedList_create(const size_t itemSize, CollectionsItemDestructorFn itemDestructor) {
	// Return null if the item size is zero
	if (itemSize == 0) {
		Logger_error("Failed to create LinkedList: Item size cannot be 0.");
		return nullptr;
	}

	// Allocate memory for the LinkedList
	LinkedList* linkedList = Memory_malloc(sizeof(*linkedList));
	if (!linkedList) {
		Logger_error("Failed to create LinkedList: Memory allocation failed for LinkedList instance.");
		return nullptr;
	}

	// Initialize the LinkedList
	linkedList->length = 0;
	linkedList->itemSize = itemSize;
	linkedList->head = nullptr;
	linkedList->tail = nullptr;
	linkedList->itemDestructor = itemDestructor;

	return linkedList;
}

void LinkedList_destroy(LinkedList* linkedList) {
	// Return if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to destroy LinkedList: LinkedList is null.");
		return;
	}

	// Release the LinkedList and its nodes
	LinkedList_clear(linkedList);
	Memory_free(linkedList);
}

size_t LinkedList_getLength(const LinkedList* linkedList) {
	// Return 0 if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to get length of LinkedList: LinkedList is null.");
		return 0;
	}

	// Return the length of the list
	return linkedList->length;
}

void LinkedList_clear(LinkedList* linkedList) {
	// Return if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to clear LinkedList: LinkedList is null.");
		return;
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
}

void* LinkedList_getItem(LinkedList* linkedList, size_t index) {
	// Return if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to get item from LinkedList: LinkedList is null.");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to get item from LinkedList: Index %zu is out of bounds for LinkedList of length %zu.",
			index, linkedList->length);
		return nullptr;
	}

	// Return the node data
	const Node* node = getNode(linkedList, index);
	return node->data;
}

void* LinkedList_insertItem(LinkedList* linkedList, const void* item, const size_t index) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to insert item to LinkedList: LinkedList is null.");
		return nullptr;
	}

	// Return if the item is null
	if (!item) {
		Logger_error("Failed to insert item to LinkedList: Item is null.");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index > linkedList->length) {
		Logger_error(
			"Failed to insert item to LinkedList: Index %zu is out of bounds for LinkedList of length %zu.",
			index, linkedList->length);
		return nullptr;
	}

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

	// Create the node to insert
	Node* node = createNode(linkedList, item);
	if (!node) { return nullptr; }
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

void* LinkedList_removeItem(LinkedList* linkedList, size_t index) {
	// Return null if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to remove item from LinkedList: LinkedList is null.");
		return nullptr;
	}
	
	// Return null if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to remove item from LinkedList: Index %zu is out of bounds for LinkedList of length %zu.",
			index, linkedList->length);
		return nullptr;
	}
	
	// Get the node at the index
	Node* node = getNode(linkedList, index);

	// Copy the item to a new memory location
	void* item = Memory_malloc(linkedList->itemSize);
	if (!item) {
		Logger_error("Failed to remove item from LinkedList: Memory allocation failed for removed item.");
		return nullptr;
	}
	Memory_copy(item, node->data, linkedList->itemSize);

	// Remove the node
	removeNode(linkedList, node);

	// Decrement the length of the LinkedList
	linkedList->length--;

	// Return the item
	return item;
}

void LinkedList_destroyItem(LinkedList* linkedList, size_t index) {
	// Return if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to destroy LinkedList item: LinkedList is null.");
		return;
	}

	// Return if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to destroy LinkedList item: Index %zu is out of bounds for LinkedList of length %zu.",
			index, linkedList->length);
		return;
	}

	// Get the node
	Node* node = getNode(linkedList, index);

	// Get the item and call its destructor if it exists
	if (linkedList->itemDestructor) {
		void* item = node->data;
		linkedList->itemDestructor(item);
	}

	// Remove the node
	removeNode(linkedList, node);
}

void* LinkedList_replaceItem(LinkedList *linkedList, const void* newItem, size_t index) {
	// Return if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to replace LinkedList item: LinkedList is null.");
		return nullptr;
	}

	// Return if the item is null
	if (!newItem) {
		Logger_error("Failed to replace LinkedList item: Item is null.");
		return nullptr;
	}

	// Return if the index is out of bounds
	if (index >= linkedList->length) {
		Logger_error(
			"Failed to replace LinkedList item: Index %zu is out of bounds for LinkedList of length %zu.",
			index, linkedList->length);
		return nullptr;
	}

	// Get the node to replace
	Node* node = getNode(linkedList, index);

	// Copy the old item to a new memory location
	void* oldItem = Memory_malloc(linkedList->itemSize);
	if (!oldItem) {
		Logger_error("Failed to replace LinkedList item: Memory allocation failed for removed item.");
		return nullptr;
	}
	Memory_copy(oldItem, node->data, linkedList->itemSize);

	// Copy the new item into the node
	Memory_copy(node->data, newItem, linkedList->itemSize);

	// Return the old item
	return oldItem;
}

LinkedListIterator LinkedList_begin(LinkedList* linkedList) {
	// Return an empty iterator if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to create LinkedList iterator: LinkedList is null.");
		return (LinkedListIterator){};
	}

	// Return an iterator starting at the beginning of the LinkedList
	return (LinkedListIterator){
		.linkedList = linkedList,
		.next = linkedList->head,
		.previous = nullptr,
	};
}

LinkedListIterator LinkedList_end(LinkedList* linkedList) {
	// Return an empty iterator if the LinkedList is null
	if (!linkedList) {
		Logger_error("Failed to create LinkedList iterator: LinkedList is null.");
		return (LinkedListIterator){};
	}

	// Return an iterator starting at the end of the LinkedList
	return (LinkedListIterator){
		.linkedList = linkedList,
		.next = nullptr,
		.previous = linkedList->tail,
	};
}

void* LinkedListIterator_next(LinkedListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to advance LinkedListIterator: LinkedListIterator is null.");
		return nullptr;
	}

	// Return null if the next node is null
	if (!iterator->next) {
		return nullptr;
	}

	// Get the next node
	Node* node = iterator->next;

	// Set the new next and previous nodes
	iterator->previous = node;
	iterator->next = node->next;

	// Return the node
	return node->data;
}

void* LinkedListIterator_previous(LinkedListIterator* iterator) {
	// Return null if the iterator is null
	if (!iterator) {
		Logger_error("Failed to regress LinkedListIterator: LinkedListIterator is null.");
		return nullptr;
	}

	// Return null if the previous node is null
	if (!iterator->previous) {
		return nullptr;
	}

	// Get the previous node
	Node* node = iterator->previous;

	// Set the new previous and next nodes
	iterator->next = node;
	iterator->previous = node->previous;

	// Return the node
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
	Memory_copy(node->data, item, linkedList->itemSize);

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