#include "renderer.h"

#include <stdlib.h>

static RendererState rendererState;

static void setDrawColor(Color color);

void Renderer_create() {
	Logger_info("Creating renderer subsystem...");

	// Create the renderer handle
	rendererState.handle = SDL_CreateRenderer(Window_getHandle(), nullptr);
	if (!rendererState.handle) {
		Logger_error("Failed to create renderer: SDL Error: %s", SDL_GetError());
		exit(-1);
	}

	// Create the lines ArrayList
	rendererState.lines = ArrayList_create(sizeof(Line), 0);

	// Set renderer properties
	SDL_SetRenderVSync(rendererState.handle, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetRenderDrawBlendMode(rendererState.handle, SDL_BLENDMODE_BLEND);
}

void Renderer_destroy() {
	Logger_info("Destroying renderer subsystem...");

	// Release SDL resources
	SDL_DestroyRenderer(rendererState.handle);
}

void Renderer_render() {
	setDrawColor(COLOR_BLACK);
	SDL_RenderClear(rendererState.handle);

	const Line* lines = ArrayList_getItems(rendererState.lines);
	for (int i = 0; i < ArrayList_getLength(rendererState.lines); i++) {
		const Line* line = &lines[i];
		setDrawColor(line->color);
		SDL_RenderLine(rendererState.handle,
			(float)line->x1, (float)line->y1,
			(float)line->x2, (float)line->y2);
	}
	ArrayList_clear(rendererState.lines);

	// Show the rendered frame
	SDL_RenderPresent(rendererState.handle);
}

void Renderer_drawLine(const int x1, const int y1, const int x2, const int y2, const Color color) {
	const Line line = { color, x1, y1, x2, y2};
	ArrayList_pushBack(rendererState.lines, &line);
}

void setDrawColor(const Color color) {
	SDL_SetRenderDrawColorFloat(rendererState.handle,
		color.r, color.g, color.b, color.a);
}
