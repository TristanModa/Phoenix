#include "resources.h"

#include <assert.h>
#include <SDL3/SDL.h>

#include "logger.h"
#include "memory.h"
#include "vtime.h"

static ResourcesState resourcesState;

static Resource* createResource(ResourceType type, const char* path);
static void destroyResource(Resource* resource);
static void destroyCacheEntry(CacheEntry* cacheEntry);

void Resources_setProperties(const ResourcesProperties* properties) {
    resourcesState.cachePurgeFrequency = properties->cachePurgeFrequency;
}

void Resources_create() {
    Logger_info("Creating resources subsystem...");
    Logger_pushIndent();

    // Initialize resources state
    resourcesState.lastCachePurgeTime = 0;

    // Create the resources cache
    resourcesState.cache = HashMap_create(sizeof(CacheEntry), (ItemDestructorFn)destroyCacheEntry);
    if (!resourcesState.cache) {
        Logger_error("Failed to create resources cache: HashMap creation failed");
        exit(EXIT_FAILURE);
    }

    Logger_popIndent();
}

void Resources_destroy() {
    Logger_info("Destroying resources subsystem...");
    Logger_pushIndent();

    // Destroy the resource cache
    HashMap_destroy(resourcesState.cache);

    Logger_popIndent();
}

void Resources_update() {
    // Purge the cache if required
    const float now = Time_getCurrentTime();
    const float timeSinceLastPurge = now - resourcesState.lastCachePurgeTime;
    if (timeSinceLastPurge > resourcesState.cachePurgeFrequency) {
        Resources_purgeCache();
        resourcesState.lastCachePurgeTime = now;
    }
}

void Resources_purgeCache() {
    // Remove any cache entries with zero references
    size_t entriesPurged = 0;
    HashMapIter it = HashMap_begin(resourcesState.cache);
    const KeyValuePair* keyValuePair;
    while ((keyValuePair = HashMapIter_getNext(&it))) {
        const CacheEntry* cacheEntry = keyValuePair->value;
        if (cacheEntry->referenceCount == 0) {
            HashMapIter_removeItem(&it, nullptr);
            entriesPurged++;
        }
    }

    // Log a message if any entries were purged
    if (entriesPurged > 0) {
        Logger_info("Purged resource cache: Unloaded %zu resources", entriesPurged);
    }
}

const Resource* Resources_getResource(const ResourceType type, const char* path) {
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
        const CacheEntry cacheEntry = {
            .resource = resource,
            .referenceCount = 0
        };
        KeyValuePair* keyValuePair = HashMap_insertItem(resourcesState.cache, path, &cacheEntry);
        if (!keyValuePair) {
            Logger_error("Failed to load resource: Failed to create cache entry");
            return nullptr;
        }

        // Set the path of the resource
        resource->path = keyValuePair->key;
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
    // Return if the provided resource is null
    if (!resource) {
        Logger_error("Failed to release resource: Resource is null");
        return;
    }

    // Return if the resource's key does not exist in the cache
    if (!HashMap_hasKey(resourcesState.cache, resource->path)) {
        Logger_error(
            "Failed to release resource: Resource of path \"%s\" does not exist in the cache",
            resource->path);
        return;
    }

    // Decrement the resource's reference count
    CacheEntry* cacheEntry = HashMap_getItem(resourcesState.cache, resource->path);
    cacheEntry->referenceCount--;
}

Resource* createResource(ResourceType type, const char* path) {

}

void destroyResource(Resource* resource) {

}

void destroyCacheEntry(CacheEntry* cacheEntry) {

    Memory_free(cacheEntry->resource);
    Memory_free(cacheEntry);
}
