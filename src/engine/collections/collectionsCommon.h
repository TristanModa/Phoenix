#ifndef ENGINE_COLLECTIONS_COMMON_H
#define ENGINE_COLLECTIONS_COMMON_H

/**
 * Return status codes for Collections C
 */
typedef enum cc_stat CC_Stat;

/**
 * A function pointer for a collection item destructor
 */
typedef void (*ItemDestructorFn)(void*);
/**
 * A function pointer for a collection forEach action
 */
typedef void (*ItemActionFn)(void*);
/**
 * A function pointer for a collection item compare function
 */
typedef int (*ItemCompareFn)(const void*, const void*);

/**
 * Checks if a condition is satisfied and logs an error and returns if it is not
 * @param condition The condition that is required to be satisfied
 * @param errorMessage The error message to write to log when the condition is not satisfied
 */
#define COLLECTIONS_REQUIRE(condition, errorMessage, ...) \
	do { \
		if (!(condition)) { \
			Logger_error(errorMessage); \
			return __VA_ARGS__; \
		} \
	} while (0)


#endif //ENGINE_COLLECTIONS_COMMON_H
