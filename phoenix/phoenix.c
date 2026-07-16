#include "phoenix.h"

#include "gfx/renderer.h"

int totalTicks = 0;
int ticksThisFrame;

void Phoenix_init() {
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