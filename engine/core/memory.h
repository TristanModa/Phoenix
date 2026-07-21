#ifndef ENGINE_CORE_MEMORY_H
#define ENGINE_CORE_MEMORY_H

#include <stddef.h>

typedef struct memoryStats {
	size_t currentAllocatedBytes;
	size_t peakAllocatedBytes;
	size_t totalAllocatedBytes;
} MemoryStats;

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
 * @param count The amount of objects to allocate memory for
 * @param size The size of one object in bytes
 * @return A pointer to the allocated memory
 */
void* Memory_calloc(size_t count, size_t size);
/**
 * Reallocates a given memory address on the heap
 * @param p A pointer to the memory to reallocate
 * @param newSize The amount of memory to allocate in bytes
 * @return A pointer to the allocated memory
 */
void* Memory_realloc(void* p, size_t newSize);
/**
 * Frees a given memory address
 * @param p A pointer to the memory to free
 */
void Memory_free(void* p);

MemoryStats Memory_getStats();

#endif //ENGINE_CORE_MEMORY_H
