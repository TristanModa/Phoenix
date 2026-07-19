#ifndef ENGINE_CORE_MEMORY_H
#define ENGINE_CORE_MEMORY_H

#include <stddef.h>

/**
 * Function pointer for a malloc function
 */
typedef void* (*MallocFn)(size_t);
/**
 * Function pointer for a calloc function
 */
typedef void* (*CallocFn)(size_t, size_t);
/**
 * Function pointer for a realloc function
 */
typedef void* (*ReallocFn)(void*, size_t);
/**
 * Function pointer for a free function
 */
typedef void  (*FreeFn)(void*);

/**
 * Function pointer for a memory free function
 */
typedef int (*MemCmpFn)(const void*, const void*, size_t);
/**
 * Function pointer for a memory copy function
 */
typedef void* (*MemCopyFn)(void*, const void*, size_t);
/**
 * Function pointer for a memory move function
 */
typedef void* (*MemMoveFn)(void*, const void*, size_t);

/**
 * Defines a configuration for the memory subsystem
 */
typedef struct memoryProperties {
	/**
	 * Function pointer for the malloc function to use
	 */
    MallocFn malloc;
	/**
	 * Function pointer for the calloc function to use
	 */
    CallocFn calloc;
	/**
	 * Function pointer for the realloc function to use
	 */
    ReallocFn realloc;
	/**
	 * Function pointer for the free function to use
	 */
    FreeFn free;

	/**
	 * Function pointer for the compare function to use
	 */
	MemCmpFn compare;
	/**
	 * Function pointer for the copy function to use
	 */
    MemCopyFn copy;
	/**
	 * Function pointer for the move function to use
	 */
	MemMoveFn move;

} MemoryProperties;

/**
 * The state of the memory subsystem
 */
typedef struct memoryState {
    /**
     * Function pointer for the malloc function to use
     */
    MallocFn malloc;
    /**
     * Function pointer for the calloc function to use
     */
    CallocFn calloc;
    /**
     * Function pointer for the realloc function to use
     */
    ReallocFn realloc;
    /**
     * Function pointer for the free function to use
     */
    FreeFn free;

	/**
	 * Function pointer for the compare function to use
	 */
	MemCmpFn compare;
    /**
	 * Function pointer for the copy function to use
	 */
    MemCopyFn copy;
    /**
     * Function pointer for the move function to use
     */
    MemMoveFn move;
} MemoryState;

/**
 * Sets the properties of the memory subsystem.
 * Should be called after Application_create but prior to Application_run.
 * @param memoryProperties The properties struct to use
 */
void Memory_setProperties(const MemoryProperties* memoryProperties);

/**
 * Initializes the memory subsystem
 */
void Memory_init();

/**
 * Allocates uninitialized memory on the heap
 * @param size The amount of memory to allocate in bytes
 * @return A pointer to the allocated memory
 */
void* Memory_malloc(size_t size);
/**
 * Allocates memory for an array of objects on the heap
 * @param num The amount of objects to allocate memory for
 * @param size The size of one object in bytes
 * @return A pointer to the allocated memory
 */
void* Memory_calloc(size_t num, size_t size);
/**
 * Reallocates a given memory address on the heap
 * @param ptr A pointer to the memory to reallocate
 * @param size The amount of memory to allocate in bytes
 * @return A pointer to the allocated memory
 */
void* Memory_realloc(void* ptr, size_t size);
/**
 * Frees a given memory address
 * @param ptr A pointer to the memory to free
 */
void Memory_free(void* ptr);

/**
 * Compares n bytes of a and b
 * @param a The first memory region
 * @param b The second memory region
 * @param n The number of bytes to compare
 * @return
 * Negative value if the first differing byte in a is less than the corresponding byte in b.
 * 0 if all bytes of a and b are equal.
 * Positive value if the first differing byte in a is greater than the corresponding byte in b.
 */
int Memory_compare(const void* a, const void* b, size_t n);
/**
 * Copies n bytes from src to dest.
 * Does not handle overlapping memory regions.
 * @param dest The destination to copy to
 * @param src The source to copy from
 * @param n The amount of bytes to copy
 * @return A pointer to dest
 */
void* Memory_copy(void* dest, const void* src, size_t n);
/**
 * Copies n bytes from src to dest.
 * Handles overlapping memory regions.
 * @param dest The destination to copy to
 * @param src The source to copy from
 * @param n The amount of bytes to copy
 * @return A pointer to dest
 */
void* Memory_move(void* dest, const void* src, size_t n);

#endif //ENGINE_CORE_MEMORY_H
