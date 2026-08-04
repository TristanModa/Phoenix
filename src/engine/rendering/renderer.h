#ifndef ENGINE_RENDERING_RENDERER_H
#define ENGINE_RENDERING_RENDERER_H

#include <SDL3/SDL.h>

constexpr size_t DEBUG_LINE_BUFFER_SIZE = 2048;

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

#endif //ENGINE_RENDERING_RENDERER_H
