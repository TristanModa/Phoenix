#ifndef ENGINE_CORE_WINDOW_H
#define ENGINE_CORE_WINDOW_H

#include <SDL3/SDL.h>

#include "types.h"

/**
 * A void function pointer for a window close callback
 */
typedef VoidFn WindowCloseCallback;

/**
 * Defines a configuration for the window subsystem
 */
typedef struct windowProperties {
    /**
     * The flags to use during window creation
     */
    SDL_WindowFlags flags;
    /**
     * The width to use during window creation
     */
    int width;
    /**
     * The height to use during window creation
     */
    int height;
} WindowProperties;

/**
 * Defines the state of the window subsystem
 */
typedef struct windowState {
    SDL_Window* handle;

    struct {
        SDL_WindowFlags flags;
        int width, height;
    } creationParameters;

    WindowCloseCallback closeCallback;
} WindowState;

/**
 * Sets the properties of the window subsystem.
 * Should be called after Application_create but prior to Application_run.
 * @param windowProperties The properties struct to use
 */
void Window_setProperties(const WindowProperties* windowProperties);

/**
 * Creates the window
 * @param title The title of the window
 * @param windowCloseCallback A function pointer that should be called when the window is closed
 * @return True on success
 */
void Window_create(const char* title, WindowCloseCallback windowCloseCallback);
/**
 * Destroys the window and frees associated resources
 */
void Window_destroy();

/**
 * Polls and handles window events
 */
void Window_pollEvents();

/**
 * Gets the SDL handle of the window.
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
