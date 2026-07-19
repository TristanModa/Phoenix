#ifndef ENGINE_GFX_RENDERER_H
#define ENGINE_GFX_RENDERER_H

#include <SDL3/SDL.h>

#include "collections/collections.h"
#include "core/core.h"
#include "colors.h"

/**
 * The state of the renderer subsystem
 */
typedef struct rendererState {
	SDL_GPUDevice* gpuDevice;

	ArrayList* lines;
} RendererState;

/**
 * Creates the renderer subsystem
 * @return True on success
 */
void Renderer_create();
/**
 * Destroys the renderer and frees associated resources
 */
void Renderer_destroy();

/**
 * Renders the current frame and draws it to the window
 */
void Renderer_render();

#endif //ENGINE_GFX_RENDERER_H
