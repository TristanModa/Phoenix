#ifndef ENGINE_CORE_RESOURCES_H
#define ENGINE_CORE_RESOURCES_H

#include <collections/collections.h>

typedef struct resourcesProperties {
    float cachePurgeFrequency;
} ResourcesProperties;

typedef enum resourceType {
    RESOURCE_TYPE_TEXT,
} ResourceType;

typedef struct resource {
    const char* path;
    ResourceType type;
    union {
        void* data;
        char* text;
    };
} Resource;

typedef struct cacheEntry {
    Resource* resource;
    size_t referenceCount;
} CacheEntry;

typedef struct resourcesState {
    HashMap* cache;

    float cachePurgeFrequency;
    float lastCachePurgeTime;
} ResourcesState;

void Resources_setProperties(const ResourcesProperties* properties);

void Resources_create();
void Resources_destroy();

void Resources_update();

void Resources_purgeCache();

const Resource* Resources_getResource(ResourceType type, const char* path);
void Resources_releaseResource(const Resource* resource);

#endif //ENGINE_CORE_RESOURCES_H
