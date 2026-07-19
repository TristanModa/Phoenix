#include "memory.h"

#include <stdlib.h>
#include <SDL3/SDL.h>

#include "logger.h"

static MemoryState memState;

void Memory_setProperties(const MemoryProperties* memoryProperties) {
    memState.malloc = memoryProperties->malloc;
    memState.calloc = memoryProperties->calloc;
    memState.realloc = memoryProperties->realloc;
    memState.free = memoryProperties->free;
    memState.compare = memoryProperties->compare;
    memState.copy = memoryProperties->copy;
    memState.move = memoryProperties->move;
}

void Memory_init() {
    Logger_info("Initializing memory subsystem...");
    Logger_pushIndent();

    // Set SDL memory functions
    SDL_SetMemoryFunctions(Memory_malloc, Memory_calloc, Memory_realloc, Memory_free);

    // Throw an error if any allocator functions are a nullptr
    if (!memState.malloc || !memState.calloc || !memState.realloc || !memState.free) {
        Logger_fatal("Failed to initialize memory subsystem: One or more allocator functions are null.");
        exit(EXIT_FAILURE);
    }

    // Throw an error if any utility functions are a nullptr
    if (!memState.compare || !memState.copy || !memState.move) {
        Logger_fatal("Failed to initialize memory subsystem: One or more utility functions are null.");
        exit(EXIT_FAILURE);
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

int Memory_compare(const void* a, const void* b, size_t n) {
    return memState.compare(a, b, n);
}

void* Memory_copy(void* dest, const void* src, const size_t n) {
    return memState.copy(dest, src, n);
}

void* Memory_move(void* dest, const void* src, const size_t n) {
    return memState.move(dest, src, n);
}