#include "resources.h"

#include <assert.h>
#include <SDL3/SDL.h>

#include "logger.h"
#include "vtime.h"
#include "collections/hashMap.h"

typedef struct cacheEntry {
    Resource* resource;
    size_t referenceCount;
} CacheEntry;

static struct {
    HashMap* cache;
    float lastCachePurgeTime;
} resourcesState;

static bool createResource(ResourceType type, const char* path, Resource* out);
static void destroyResource(Resource* resource);
static void destroyCacheEntry(CacheEntry* cacheEntry);

void Resources_create() {
    Logger_info("Creating resource manager...");
    Logger_pushIndent();

    // Initialize the resources state
    resourcesState = (typeof(resourcesState)){};

    // Create the resource cache
    resourcesState.cache = HashMap_create(sizeof(CacheEntry), (ItemDestructorFn)destroyCacheEntry);
    if (!resourcesState.cache) {
        Logger_fatal("Failed to create resource cache: HashMap creation failed");
        exit(EXIT_FAILURE);
    }

    Logger_popIndent();
}

void Resources_destroy() {
    HashMap_destroy(resourcesState.cache);
}

void Resources_update() {
    // Purge the cache if the time since the last purge is greater than the interval
    const float currentTime = Time_getCurrentTime();
    const float timeSinceLastPurge = currentTime - resourcesState.lastCachePurgeTime;
    if (timeSinceLastPurge >= RESOURCE_CACHE_PURGE_INTERVAL) {
        Resources_purgeCache();
    }
}

void Resources_purgeCache() {
    // Iterate through the cache and remove any elements that are unused
    size_t entriesPurged = 0;
    const KeyValuePair* kvp;
    HashMapIter it = HashMap_begin(resourcesState.cache);
    while ((kvp = HashMapIter_getNext(&it))) {
        CacheEntry* cacheEntry = kvp->value;
        if (cacheEntry->referenceCount == 0) {
            HashMapIter_removeItem(&it, nullptr);
            entriesPurged++;
        }
    }

    // Log the purge results
    if (entriesPurged != 0) {
        Logger_info("Purged resource cache (%zu entries purged)", entriesPurged);
    }
}

const Resource* Resources_getResource(ResourceType type, const char* path) {
    // Load the resource and add it to the cache if it has not already been loaded
    if (!HashMap_hasKey(resourcesState.cache, path)) {
        // Allocate and create the resource
        Resource* resource = malloc(sizeof(*resource));
        if (!resource) {
            Logger_error("Failed to create resource: Memory allocation failed");
            return nullptr;
        }
        if (!createResource(type, path, resource)) {
            Logger_error("Failed to create resource: SDL Error: %s", SDL_GetError());
            free(resource);
            return nullptr;
        }

        // Create and insert a cache entry for the resource
        CacheEntry cacheEntry = {
            .resource = resource
        };
        KeyValuePair* kvp = HashMap_insertItem(resourcesState.cache, path, &cacheEntry);
        if (!kvp) {
            Logger_error("Failed to load resource: Failed to create cache entry");
            return nullptr;
        }

        // Set the path of the resource
        resource->path = kvp->key;
    }

    // Get the resource from cache
    CacheEntry* cacheEntry = HashMap_getItem(resourcesState.cache, path);
    assert(cacheEntry);

    // Increment the cache entry's reference count
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

bool createResource(ResourceType type, const char* path, Resource* out) {
    assert(out);

    // Load the resource data
    out->type = type;
    switch (out->type) {
        default:
            out->data = SDL_LoadFile(path, nullptr);
            break;
    }

    // Return whether the data was successfully loaded
    return out->data;
}

void destroyResource(Resource* resource) {
    switch (resource->type) {
        default:
            SDL_free(resource->data);
    }
    free(resource);
}

void destroyCacheEntry(CacheEntry* cacheEntry) {
    destroyResource(cacheEntry->resource);
    free(cacheEntry);
}
