#include "resources.h"

#include <assert.h>
#include <SDL3/SDL.h>

#include "logger.h"
#include "memory.h"

static ResourcesState resourcesState;

void Resources_init() {
    Logger_info("Initializing resources subsystem...");
    Logger_pushIndent();

    // Create the resources cache
    resourcesState.cache = HashMap_create(sizeof(CacheEntry), nullptr);
    if (!resourcesState.cache) {
        // TODO: log
        exit(EXIT_FAILURE);
    }

    Logger_popIndent();
}

const Resource* Resources_loadResource(ResourceType type, const char* path) {
    // Load the resource and add it to the cache if it has not already been loaded
    if (!HashMap_hasKey(resourcesState.cache, path)) {
        // Load the resource data from disk
        void* resourceData = SDL_LoadFile(path, nullptr);
        if (!resourceData) {
            Logger_error("Failed to load resource: SDL Error: %s", SDL_GetError());
            return nullptr;
        }

        // Create the resource
        Resource* resource = Memory_malloc(sizeof(*resource));
        if (!resource) {
            Logger_error("Failed to load resource: Memory allocation failed");
            SDL_free(resourceData);
            return nullptr;
        }
        resource->type = type;
        resource->data = resourceData;

        // Create and insert the cache entry for the resource
        CacheEntry cacheEntry = {
            .resource = resource,
            .referenceCount = 0
        };
        if (!HashMap_insertItem(resourcesState.cache, path, &cacheEntry)) {
            Logger_error("Failed to load resource: Failed to insert resource cache entry");
            return nullptr;
        }
    }

    // Get the resource from cache
    CacheEntry* cacheEntry = HashMap_getItem(resourcesState.cache, path);
    assert(cacheEntry);

    // Increment the entry's reference count
    cacheEntry->referenceCount++;

    // Return the resource
    return cacheEntry->resource;
}

void Resources_releaseResource(const Resource* resource) {
    // Return
    if (!HashMap_hasKey(resourcesState.cache, )) {

    }
}