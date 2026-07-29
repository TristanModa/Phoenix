#include "phoenix.h"

#include "engine/logger.h"
#include "engine/resources.h"

void Phoenix_create() {
    const Resource* a = Resources_getResource(RESOURCE_TYPE_TEXT, "resources/a.txt");
    Logger_info("%s", a->text);
    Resources_releaseResource(a);
}

void Phoenix_destroy() {
}

void Phoenix_update() {
}

void Phoenix_tick() {
}

void Phoenix_render() {
}
