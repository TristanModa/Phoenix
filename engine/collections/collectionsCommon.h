#ifndef ENGINE_COLLECTIONS_COMMON_H
#define ENGINE_COLLECTIONS_COMMON_H

#define COLLECTIONS_ERR_ARRAY_LIST "ArrayList"
#define COLLECTIONS_ERR_LINKED_LIST "LinkedList"

#define COLLECTIONS_ERR_FAILED_CREATE "Failed to create instance of %s: "
#define COLLECTIONS_ERR_FAILED_DESTROY "Failed to destroy instance of %s: "
#define COLLECTIONS_ERR_FAILED_BOUNDS_CHECK "Failed to check bounds of %s: "
#define COLLECTIONS_ERR_FAILED_GET_LENGTH "Failed to get length of %s: "
#define COLLECTIONS_ERR_FAILED_CLEAR "Failed to clear %s: "
#define COLLECTIONS_ERR_FAILED_INSERT "Failed to insert item to %s: "
#define COLLECTIONS_ERR_FAILED_REMOVE "Failed to remove item from %s: "
#define COLLECTIONS_ERR_FAILED_GET "Failed to get item from %s: "
#define COLLECTIONS_ERR_FAILED_SET "Failed to set value of item in %s: "
#define COLLECTIONS_ERR_FAILED_PUSH "Failed to push item to %s: "
#define COLLECTIONS_ERR_FAILED_POP "Failed to pop item from %s: "
#define COLLECTIONS_ERR_FAILED_FIND "Failed to find item from %s: "

#define COLLECTIONS_ERR_MALLOC_FAIL "Memory allocation for %s failed."
#define COLLECTIONS_ERR_NULLPTR "%s is a nullptr."
#define COLLECTIONS_ERR_ITEM_SIZE_ZERO "Item size cannot be 0."
#define COLLECTIONS_ERR_INDEX_OOB "Index %zu out of bounds for %s of length %zu."
#define COLLECTIONS_ERR_POP_EMPTY "Cannot pop from %s with length 0."

#define COLLECTIONS_FATAL_IF(condition, error, ...) \
	do { \
		if (condition) { \
			Logger_fatal(error, __VA_ARGS__); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

/**
 * A function pointer for a comparison function between two collection items
 */
typedef int (*CollectionsComparisonFn)(const void*, const void*);

#endif //ENGINE_COLLECTIONS_COMMON_H
