#include "renderer.h"

#include "core/core.h"

static RendererState rendererState;

bool Renderer_create() {
	Logger_info("Creating renderer...");

	// Create the renderer handle
	rendererState.handle = SDL_CreateRenderer(Window_getHandle(), nullptr);
	if (!rendererState.handle) {
		Logger_error("Failed to create renderer handle: %s", SDL_GetError());
		return false;
	}

	// Set renderer properties
	SDL_SetRenderVSync(rendererState.handle, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetRenderDrawBlendMode(rendererState.handle, SDL_BLENDMODE_BLEND);

	return true;
}

void Renderer_destroy() {
	// Release SDL resources
	SDL_DestroyRenderer(rendererState.handle);
}

void Renderer_render() {

}
