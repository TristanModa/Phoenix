#include "window.h"

static WindowState windowState;

void Window_create(const char *title, const ExitCallback exitCallback) {
    // Create the window handle
    windowState.handle = SDL_CreateWindow(
        title,
        1280, 720,
        SDL_WINDOW_HIDDEN);

    // Set additional window properties
    SDL_SetWindowPosition(windowState.handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Set the exit callback
    windowState.exitCallback = exitCallback;
}

void Window_destroy() {
    SDL_DestroyWindow(windowState.handle);
}

void Window_pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                windowState.exitCallback();
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