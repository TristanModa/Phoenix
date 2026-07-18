#include "phoenix.h"

#include <vulfEngine.h>

#include "constants.h"

void Phoenix_init() {
}

void Phoenix_destroy() {
}

void Phoenix_update() {
    Logger_info("FPS: %f", Time_getFPS());
}

void Phoenix_tick() {

}

void Phoenix_render() {
    Renderer_drawLine(0, 0, 1279, 719, COLOR_RED);
}