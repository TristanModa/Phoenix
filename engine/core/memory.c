#include "memory.h"

#include <mimalloc.h>
#include <mimalloc-stats.h>

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