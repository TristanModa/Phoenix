#ifndef ENGINE_GFX_COLORS_H
#define ENGINE_GFX_COLORS_H

#include <SDL3/SDL.h>

typedef SDL_FColor Color;

constexpr Color COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr Color COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};

constexpr Color COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
constexpr Color COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
constexpr Color COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};

constexpr Color COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
constexpr Color COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
constexpr Color COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};

constexpr Color COLOR_RED_DARK = {0.5f, 0.0f, 0.0f, 1.0f};

#endif //ENGINE_GFX_COLORS_H
