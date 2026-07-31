#include "window.h"

#include <stdlib.h>

#include "logger.h"

static struct {
    SDL_Window* handle;
    VoidFn closeCallback;
} windowState;

void Window_create(const char* title, const VoidFn windowCloseCallback) {
    Logger_info("Creating window...");
    Logger_pushIndent();

    // Create the window handle
    windowState.handle = SDL_CreateWindow(title, 1280, 720, SDL_WINDOW_HIDDEN);
    if (!windowState.handle) {
        Logger_error("Failed to create window: SDL Error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Set additional window properties
    SDL_SetWindowPosition(windowState.handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Set the close callback
    windowState.closeCallback = windowCloseCallback;

    // Pop the log indent
    Logger_popIndent();
}

void Window_destroy() {
    SDL_DestroyWindow(windowState.handle);
}

void Window_pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                if (windowState.closeCallback) {
                    windowState.closeCallback();
                }
                break;
            default:
                break;
        }
    }
}

SDL_Window* Window_getHandle() {
    return windowState.handle;
}

void Window_show() {
    Window_setVisibility(true);
}

void Window_setVisibility(const bool visible) {
    if (visible) {
        SDL_ShowWindow(windowState.handle);
    } else {
        SDL_HideWindow(windowState.handle);
    }
}