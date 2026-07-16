#include "phoenix.h"

#include "gfx/renderer.h"

void Phoenix_init() {
    Logger_debug("Debug");
    Logger_info("Info");
    Logger_warning("Warning");
    Logger_error("Error");
    Logger_fatal("Fatal");
    Logger_log(-1, "Unknown");
}

void Phoenix_destroy() {
}

void Phoenix_update() {
}

void Phoenix_tick() {
}

void Phoenix_render() {
    Renderer_drawLine(0, 0, 1279, 719, COLOR_RED);
}