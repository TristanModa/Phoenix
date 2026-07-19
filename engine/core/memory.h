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
 * Defines a configuration for the memory subsystem
 */
typedef struct memoryProperties {
    MallocFn malloc;
    CallocFn calloc;
    ReallocFn realloc;
    FreeFn free;
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

#endif //ENGINE_CORE_MEMORY_H
