#include "memory.h"

#include <SDL3/SDL.h>
#include <mimalloc.h>
#include <mimalloc-stats.h>

#include "logger.h"

static void mimallocOutputFunction(const char* msg, void* _);

void Memory_init() {
    // Set mimalloc output function
    mi_register_output(mimallocOutputFunction, nullptr);

    // Set SDL memory functions
    SDL_SetMemoryFunctions(Memory_malloc, Memory_calloc, Memory_realloc, Memory_free);
}

void* Memory_malloc(const size_t size) {
    return mi_malloc(size);
}

void* Memory_calloc(const size_t count, const size_t size) {
    return mi_calloc(count, size);
}

void* Memory_realloc(void* p, const size_t newSize) {
    return mi_realloc(p, newSize);
}

void Memory_free(void* p) {
    mi_free(p);
}

MemoryStats Memory_getStats() {
    // Get stats from the allocator
    mi_stats_t stats;
    stats.size = sizeof(mi_stats_t);
    stats.version = MI_STAT_VERSION;
    if (!mi_stats_get(&stats)) {
        return (MemoryStats){};
    }

    // Construct and return memory stats
    return (MemoryStats){
        .currentAllocatedBytes = stats.malloc_normal.current + stats.malloc_huge.current,
        .peakAllocatedBytes = stats.malloc_normal.peak + stats.malloc_huge.peak,
        .totalAllocatedBytes = stats.malloc_normal.total + stats.malloc_huge.total
    };
}

void mimallocOutputFunction(const char* msg, void* _) {
    // Suppress messages containing only a newline or mimalloc: error
    if (strcmp(msg, "\n") == 0 || strcmp(msg, "mimalloc: error: ") == 0) {
        return;
    }

    // Create a cleaned message
    const size_t len = strlen(msg);
    char cleanedMsg[len];
    strcpy(cleanedMsg, msg);
    if (cleanedMsg[len - 1] == '\n') {
        cleanedMsg[len - 1] = '\0';
    }

    // Output the error
    Logger_error("Mimalloc Error: %s", cleanedMsg);
}