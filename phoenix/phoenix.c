#include "phoenix.h"

#include <unistd.h>

#include "core/logger.h"

int totalTicksThisFrame;

void Phoenix_init() {
}

void Phoenix_destroy() {
}

void Phoenix_update() {
    totalTicksThisFrame = 0;
    sleep(1);
}

void Phoenix_tick() {
    totalTicksThisFrame++;
}

void Phoenix_render() {
    Logger_info("Total ticks this frame: %d", totalTicksThisFrame);
}