#include "phoenix.h"

#include "gfx/renderer.h"

int ticksThisFrame;

void Phoenix_init() {
}

void Phoenix_destroy() {
}

void Phoenix_update() {
    ticksThisFrame = 0;
}

void Phoenix_tick() {
    ticksThisFrame++;
}

void Phoenix_render() {
    Logger_debug("Ticks this frame: %d", ticksThisFrame);
    Renderer_drawLine(0, 0, 1279, 719, COLOR_RED);
}