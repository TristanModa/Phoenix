#ifndef ENGINE_GFX_COLORS_H
#define ENGINE_GFX_COLORS_H

#include <SDL3/SDL.h>

/**
 * An RGBA color with channel values ranging from 0-1
 */
typedef SDL_FColor Color;

/**
 * Shorthand for Color(1, 1, 1, 1)
 */
constexpr Color COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
/**
 * Shorthand for Color(0, 0, 0, 1)
 */
constexpr Color COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};

/**
 * Shorthand for Color(0.5, 0.5, 0.5, 1)
 */
constexpr Color COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};
/**
 * Shorthand for Color(0.25, 0.25, 0.25, 1)
 */
constexpr Color COLOR_GRAY_DARK = {0.25f, 0.25f, 0.25f, 1.0f};
/**
 * Shorthand for Color(0.75, 0.75, 0.75, 1)
 */
constexpr Color COLOR_GRAY_LIGHT = {0.75f, 0.75f, 0.75f, 1.0f};

/**
 * Shorthand for Color(1, 0, 0, 1)
 */
constexpr Color COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0, 1, 0, 1)
 */
constexpr Color COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0, 0, 1, 1)
 */
constexpr Color COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
/**
 * Shorthand for Color(0.5, 0, 0, 1)
 */
constexpr Color COLOR_RED_DARK = {0.5f, 0.0f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0, 0.5, 0, 1)
 */
constexpr Color COLOR_GREEN_DARK = {0.0f, 0.5f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0, 0, 0.5, 1)
 */
constexpr Color COLOR_BLUE_DARK = {0.0f, 0.0f, 0.5f, 1.0f};
/**
 * Shorthand for Color(1, 0.5, 0.5, 1)
 */
constexpr Color COLOR_RED_LIGHT = {1.0f, 0.5f, 0.5f, 1.0f};
/**
 * Shorthand for Color(0.5, 1, 0.5, 1)
 */
constexpr Color COLOR_GREEN_LIGHT = {0.5f, 1.0f, 0.5f, 1.0f};
/**
 * Shorthand for Color(0.5, 0.5, 1, 1)
 */
constexpr Color COLOR_BLUE_LIGHT = {0.5f, 0.5f, 1.0f, 1.0f};

/**
 * Shorthand for Color(0, 1, 1, 1)
 */
constexpr Color COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
/**
 * Shorthand for Color(1, 0, 1, 1)
 */
constexpr Color COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
/**
 * Shorthand for Color(1, 1, 0, 1)
 */
constexpr Color COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0, 0.5, 0.5, 1)
 */
constexpr Color COLOR_CYAN_DARK = {0.0f, 0.5f, 0.5f, 1.0f};
/**
 * Shorthand for Color(0.5, 0.5, 0, 1)
 */
constexpr Color COLOR_MAGENTA_DARK = {0.5f, 0.0f, 0.5f, 1.0f};
/**
 * Shorthand for Color(0.5, 0.5, 0, 1)
 */
constexpr Color COLOR_YELLOW_DARK = {0.5f, 0.5f, 0.0f, 1.0f};
/**
 * Shorthand for Color(0.5, 1, 1, 1)
 */
constexpr Color COLOR_CYAN_LIGHT = {0.5f, 1.0f, 1.0f, 1.0f};
/**
 * Shorthand for Color(1, 0.5, 1, 1)
 */
constexpr Color COLOR_MAGENTA_LIGHT = {1.0f, 0.5f, 1.0f, 1.0f};
/**
 * Shorthand for Color(1, 1, 0.5, 1)
 */
constexpr Color COLOR_YELLOW_LIGHT = {1.0f, 1.0f, 0.5f, 1.0f};

#endif //ENGINE_GFX_COLORS_H
