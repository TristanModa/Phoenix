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

	x += (float)Input_getButtonState(BUTTON_RIGHT)->held * Time_getDeltaTime();
	x -= (float)Input_getButtonState(BUTTON_LEFT)->held * Time_getDeltaTime();

	y += (float)Input_getButtonState(BUTTON_UP)->held * Time_getDeltaTime();
	y -= (float)Input_getButtonState(BUTTON_DOWN)->held * Time_getDeltaTime();

	Renderer_setCameraPosition(x, y);
}

void Phoenix_tick() {

}

void Phoenix_render() {
	Renderer_drawDebugLine(0, 0, VIRTUAL_DISPLAY_WIDTH, VIRTUAL_DISPLAY_HEIGHT, COLOR_BLUE);
}
