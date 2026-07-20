#ifndef ENGINE_COLLECTIONS_COMMON_H
#define ENGINE_COLLECTIONS_COMMON_H

/**
 * A function pointer for a collection item destructor
 * @param item The item to destroy
 */
typedef void (*CollectionsItemDestructorFn)(void* item);
/**
 * A function pointer for a collection forEach action
 * @param item The item to execute the action on
 */
typedef void (*CollectionsForEachActionFn)(void* item);
/**
 * A function pointer for a collection item compare function
 * @param item The item to compare
 * @param key The key to compare the item against
 */
typedef int (*CollectionsCompareFn)(const void* item, const void* key);

#endif //ENGINE_COLLECTIONS_COMMON_H
