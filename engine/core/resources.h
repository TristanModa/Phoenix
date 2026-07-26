#ifndef ENGINE_CORE_RESOURCES_H
#define ENGINE_CORE_RESOURCES_H

#include <collections/collections.h>

typedef struct resourcesState {
    HashMap* cache;
} ResourcesState;

typedef enum resourceType {
    RESOURCE_TYPE_TEXT,
} ResourceType;

typedef struct resource {
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

void Resources_init();

const Resource* Resources_loadResource(ResourceType type, const char* path);
void Resources_releaseResource(const Resource* resource);

#endif //ENGINE_CORE_RESOURCES_H
