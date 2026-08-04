#ifndef ENGINE_RENDERING_RENDERER_H
#define ENGINE_RENDERING_RENDERER_H

#include <SDL3/SDL.h>

#include "colors.h"

constexpr size_t MAX_DEBUG_LINE_COUNT = 2048;
static_assert(MAX_DEBUG_LINE_COUNT % 2 == 0);

/**
 * Creates the renderer
 * @param windowHandle The handle of the window the renderer should draw to
 */
void Renderer_create(SDL_Window* windowHandle);
/**
 * Destroys the renderer and frees associated resources
 */
void Renderer_destroy();

/**
 * Renders the current frame to the window
 */
void Renderer_render();

/**
 * Gets the SDL handle of the renderer's GPU device
 * @return The GPU device
 */
SDL_GPUDevice* Renderer_getGPUDevice();

/**
 * Adds a debug line to the debug lines list and draws it this frame
 */
void Renderer_drawDebugLine(float x1, float y1, float x2, float y2, Color color);

#endif //ENGINE_RENDERING_RENDERER_H
