#include "memory.h"

#include <SDL3/SDL.h>

static MemoryState memState;

void Memory_init() {
    // Set SDL memory functions
    SDL_SetMemoryFunctions(Memory_malloc, Memory_calloc, Memory_realloc, Memory_free);
}

void Memory_setAllocators(const MallocFunc malloc, const CallocFunc calloc, const ReallocFunc realloc,
    const FreeFunc free) {
    // Set the memory function pointers
    memState.malloc = malloc;
    memState.calloc = calloc;
    memState.realloc = realloc;
    memState.free = free;
}

void* Memory_malloc(const size_t size) {
    return memState.malloc(size);
}

void* Memory_calloc(const size_t num, const size_t size) {
    return memState.calloc(num, size);
}

void* Memory_realloc(void* mem, const size_t size) {
    return memState.realloc(mem, size);
}

void Memory_free(void* mem) {
    memState.free(mem);
}