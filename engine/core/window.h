#ifndef ENGINE_CORE_WINDOW_H
#define ENGINE_CORE_WINDOW_H

#include <SDL3/SDL.h>

#include "types.h"

typedef void (*ExitCallback)(int);

typedef struct windowState {
    SDL_Window* handle;
    ExitCallback exitCallback;
} WindowState;

bool Window_create(const char* title, ExitCallback exitCallback);
void Window_destroy();

void Window_pollEvents();

SDL_Window* Window_getHandle();

void Window_show();
void Window_setVisibility(bool visible);

#endif //ENGINE_CORE_WINDOW_H
