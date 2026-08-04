#include "resources.h"

void* Resources_get(ResourceID resourceID, size_t* size) {
    // Get the data
    unsigned int out = 0;
    void* data = getResourceData(resourceID, &out);

    // Return the data
    if (size) *size = (size_t)out;
    return data;
}