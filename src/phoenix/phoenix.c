#include "phoenix.h"

#include "engine/core/input.h"
#include "engine/core/vtime.h"
#include "engine/rendering/renderer.h"

void Phoenix_create() {

}

void Phoenix_destroy() {
}

void Phoenix_update() {
	static float x = 0;
	static float y = 0;

	x += (float)Input_getButtonState(BUTTON_RIGHT)->held * 2.0f * Time_getDeltaTime();
	x -= (float)Input_getButtonState(BUTTON_LEFT)->held * 2.0f * Time_getDeltaTime();

	y -= (float)Input_getButtonState(BUTTON_UP)->held * 2.0f * Time_getDeltaTime();
	y += (float)Input_getButtonState(BUTTON_DOWN)->held * 2.0f * Time_getDeltaTime();

	Renderer_setCameraPosition(x, y);
}

void Phoenix_tick() {

}

void Phoenix_render() {
	for (int y = 0; y < VIRTUAL_DISPLAY_HEIGHT; y += 4) {
		for (int x = 0; x < VIRTUAL_DISPLAY_WIDTH; x += 4) {
			Renderer_drawDebugLine(x, y, x + 1, y + 1, COLOR_BLUE);
		}
	}
}
