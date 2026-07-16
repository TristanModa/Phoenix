#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "core/types.h"

// Log
#define LOG_FILE_PATH "phoenix.log"

// Time
constexpr float TARGET_TICKS_PER_SECOND = 240;

// Window
constexpr SDL_WindowFlags WINDOW_FLAGS = SDL_WINDOW_FULLSCREEN;
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 720;

// Input
typedef enum buttonID : u8 {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_COUNT
} ButtonID;
constexpr Binding DEFAULT_BINDINGS[BUTTON_COUNT] = {
    [BUTTON_UP]    = { .keyboard = SDL_SCANCODE_UP    },
    [BUTTON_DOWN]  = { .keyboard = SDL_SCANCODE_DOWN  },
    [BUTTON_LEFT]  = { .keyboard = SDL_SCANCODE_LEFT  },
    [BUTTON_RIGHT] = { .keyboard = SDL_SCANCODE_RIGHT },
};


#endif //CONSTANTS_H
