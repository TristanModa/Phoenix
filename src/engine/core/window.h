#ifndef ENGINE_CORE_WINDOW_H
#define ENGINE_CORE_WINDOW_H

#include <SDL3/SDL.h>

#include "types.h"

/**
 * Creates the window
 * @param title The title of the window
 * @param windowCloseCallback A function pointer that should be called when the window is closed
 * @return True on success
 */
void Window_create(const char* title, VoidFn windowCloseCallback);
/**
 * Destroys the window and frees associated resources
 */
void Window_destroy();

/**
 * Gets the SDL handle of the window
 * @return A pointer to the SDL handle of the window
 */
SDL_Window* Window_getHandle();

/**
 * Sets the window to visible
 */
void Window_show();
/**
 * Sets the visibility of the window
 * @param visible Whether the window is visible
 */
void Window_setVisibility(bool visible);

#endif //ENGINE_CORE_WINDOW_H
