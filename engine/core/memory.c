#include "memory.h"

#include <SDL3/SDL.h>

static MemoryState memState;

void Memory_setAllocators(const MallocFunc malloc, const CallocFunc calloc, const ReallocFunc realloc,
    const FreeFunc free) {
    // Set the function pointers
    memState.malloc = malloc;
    memState.calloc = calloc;
    memState.realloc = realloc;
    memState.free = free;

    // Set SDL memory functions
    SDL_SetMemoryFunctions(memState.malloc, memState.calloc, memState.realloc, memState.free);
}

void* Memory_malloc(const size_t size) {
    return memState.malloc(size);
}

void* Memory_calloc(const size_t nmemb, const size_t size) {
    return memState.calloc(nmemb, size);
}

void* Memory_realloc(void* mem, const size_t size) {
    return memState.realloc(mem, size);
}

void Memory_free(void* mem) {
    memState.free(mem);
}