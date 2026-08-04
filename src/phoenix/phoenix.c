#include "phoenix.h"

#include "engine/core/vtime.h"
#include "engine/rendering/renderer.h"

void Phoenix_create() {

}

void Phoenix_destroy() {
}

void Phoenix_update() {


}

void Phoenix_tick() {

}

void Phoenix_render() {
	for (int i = 0; i < Time_getCurrentTick(); i++) {
		Renderer_drawDebugLine(-1.0f, -1.0f, (float)i / 1000.0f, 1.0f, COLOR_BLUE);
	}
}
