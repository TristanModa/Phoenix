#ifndef ENGINE_RENDERING_RENDERER_H
#define ENGINE_RENDERING_RENDERER_H

#include <SDL3/SDL.h>

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

#endif //ENGINE_RENDERING_RENDERER_H
