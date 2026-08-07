#ifndef ENGINE_RENDERING_RENDERER_H
#define ENGINE_RENDERING_RENDERER_H

#include <SDL3/SDL.h>

#include "colors.h"

constexpr size_t VIRTUAL_DISPLAY_WIDTH = 16;
constexpr size_t VIRTUAL_DISPLAY_HEIGHT = 9;
constexpr size_t VIRTUAL_DISPLAY_TEXTURE_PADDING = 1;
constexpr float VIRTUAL_DISPLAY_ASPECT_RATIO = (float)VIRTUAL_DISPLAY_WIDTH / (float)VIRTUAL_DISPLAY_HEIGHT;

constexpr size_t MAX_DEBUG_LINE_COUNT = 2048 * 2;
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
 * Sets the position of the renderer's camera
 * @param x The X position of the camera
 * @param y The Y position of the camera
 */
void Renderer_setCameraPosition(float x, float y);

/**
 * Adds a debug line to the debug lines list and draws it this frame
 */
void Renderer_drawDebugLine(int x1, int y1, int x2, int y2, Color color);

#endif //ENGINE_RENDERING_RENDERER_H
