#ifndef ENGINE_CORE_RESOURCES_H
#define ENGINE_CORE_RESOURCES_H

#include <stddef.h>
#include <resourceData.h>

#include "types.h"

void* Resources_get(ResourceID resourceID, size_t* size);

#endif //ENGINE_CORE_RESOURCES_H
