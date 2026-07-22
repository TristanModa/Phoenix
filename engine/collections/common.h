#ifndef ENGINE_COLLECTIONS_COMMON_H
#define ENGINE_COLLECTIONS_COMMON_H

/**
 * A function pointer for a collection item destructor
 */
typedef void (*CollectionsItemDestructorFn)(void*);
/**
 * A function pointer for a collection forEach action
 */
typedef void (*CollectionsForEachActionFn)(void*);
/**
 * A function pointer for a collection item compare function
 */
typedef int (*CollectionsCompareFn)(const void*, const void*);

#endif //ENGINE_COLLECTIONS_COMMON_H
