#include "memory.h"

#include <stdlib.h>
#include <SDL3/SDL.h>

#include "logger.h"

static MemoryState memState;

void Memory_setProperties(const MemoryProperties *memoryProperties) {
    memState.malloc = memoryProperties->mallocFunc;
    memState.calloc = memoryProperties->callocFunc;
    memState.realloc = memoryProperties->reallocFunc;
    memState.free = memoryProperties->freeFunc;
}

void Memory_init() {
    Logger_info("Initializing memory subsystem...");
    Logger_pushIndent();

    // Set SDL memory functions
    SDL_SetMemoryFunctions(Memory_malloc, Memory_calloc, Memory_realloc, Memory_free);

    // Throw an error if any allocator functions are null
    if (!memState.malloc || !memState.calloc || !memState.realloc || !memState.realloc) {
        Logger_fatal("Failed to initialize memory subsystem: One or more allocator functions are null.");
        exit(-1);
    }

    // Pop the log indent
    Logger_popIndent();
}

void* Memory_malloc(const size_t size) {
    return memState.malloc(size);
}

void* Memory_calloc(const size_t num, const size_t size) {
    return memState.calloc(num, size);
}

void* Memory_realloc(void* ptr, const size_t size) {
    return memState.realloc(ptr, size);
}

void Memory_free(void* ptr) {
    memState.free(ptr);
}