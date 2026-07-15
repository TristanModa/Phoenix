#ifndef ENGINE_CORE_MEMORY_H
#define ENGINE_CORE_MEMORY_H

#include "types.h"

typedef void* (*MallocFunc)(size_t);
typedef void* (*CallocFunc)(size_t, size_t);
typedef void* (*ReallocFunc)(void*, size_t);
typedef void  (*FreeFunc)(void*);

typedef struct memoryState {
    MallocFunc malloc;
    CallocFunc calloc;
    ReallocFunc realloc;
    FreeFunc free;
} MemoryState;

void Memory_setAllocators(MallocFunc malloc, CallocFunc calloc, ReallocFunc realloc, FreeFunc free);

void* Memory_malloc(size_t size);
void* Memory_calloc(size_t nmemb, size_t size);
void* Memory_realloc(void* mem, size_t size);
void Memory_free(void* mem);

#endif //ENGINE_CORE_MEMORY_H
