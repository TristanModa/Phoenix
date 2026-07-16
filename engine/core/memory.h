#ifndef ENGINE_CORE_MEMORY_H
#define ENGINE_CORE_MEMORY_H

#include <stddef.h>

/**
 * Function pointer for a malloc function
 */
typedef void* (*MallocFunc)(size_t);
/**
 * Function pointer for a calloc function
 */
typedef void* (*CallocFunc)(size_t, size_t);
/**
 * Function pointer for a realloc function
 */
typedef void* (*ReallocFunc)(void*, size_t);
/**
 * Function pointer for a free function
 */
typedef void  (*FreeFunc)(void*);

/**
 * Defines a configuration for the memory subsystem
 */
typedef struct memoryProperties {
    MallocFunc mallocFunc;
    CallocFunc callocFunc;
    ReallocFunc reallocFunc;
    FreeFunc freeFunc;
} MemoryProperties;

/**
 * Defines the state of the memory subsystem
 */
typedef struct memoryState {
    MallocFunc malloc;
    CallocFunc calloc;
    ReallocFunc realloc;
    FreeFunc free;
} MemoryState;

/**
 * Sets the properties of the memory subsystem.
 * Should only be called prior to running the application via Application_run.
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
