#include "phoenix.h"

#include <vulfEngine.h>

#include "constants.h"

int totalTicks = 0;
int ticksThisFrame;

void Phoenix_init() {
}

void Phoenix_destroy() {
}

void Phoenix_update() {
    const ButtonState state = *Input_getButtonState(BUTTON_COUNT);
    Logger_info("%i, %i, %i", state.pressed, state.held, state.released);
}

void Phoenix_tick() {

}

void Phoenix_render() {
    Renderer_drawLine(0, 0, 1279, 719, COLOR_RED);
}