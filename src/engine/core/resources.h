#ifndef ENGINE_CORE_RESOURCES_H
#define ENGINE_CORE_RESOURCES_H

#include "types.h"

/**
 * On what interval the resource cache should unload unused resources in seconds
 */
constexpr float RESOURCE_CACHE_PURGE_INTERVAL = 60;

/**
 * The base path to load resources from
 */
constexpr char RESOURCES_BASE_PATH[] = "resources/";

/**
 * Specifies the format of a system resource
 */
typedef enum resourceType : u8 {
    RESOURCE_TYPE_BINARY,   /**< Raw unformatted bytes */
    RESOURCE_TYPE_TEXT,     /**< Plain text */
} ResourceType;

/**
 * A resource loaded and managed by the resource manager
 */
typedef struct resource {
    const char* path;   /**< The file path to the resource */
    ResourceType type;  /**< The data type of the resource */
    size_t size;        /**< The size of the resource in bytes */
    union {
        void* data;     /**< The raw data of the resource */
        char* text;     /**< The text data of the resource */
    };
} Resource;

/**
 * Creates the resource manager
 */
void Resources_create();
/**
 * Destroys the resource manager and frees associated resources
 */
void Resources_destroy();

/**
 * Updates the resource manager
 */
void Resources_update();

/**
 * Removes and unloads any unused resources from the resource cache
 */
void Resources_purgeCache();

/**
 * Gets a resource from the resource cache, loading it into the cache if it was not already loaded
 * @param type The type of the resource to get
 * @param path The path of the resource to get
 * @return A const reference to the resource, or null on failure.
 * The caller should release the returned resource when it is no longer in use using Resources_releaseResource().
 */
const Resource* Resources_getResource(ResourceType type, const char* path);

/**
 * Releases the specified resource
 * @param resource The resource to release
 */
void Resources_releaseResource(const Resource* resource);

#endif //ENGINE_CORE_RESOURCES_H
