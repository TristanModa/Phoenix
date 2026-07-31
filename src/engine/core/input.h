#ifndef ENGINE_CORE_INPUT_H
#define ENGINE_CORE_INPUT_H

#include <SDL3/SDL.h>

#include "types.h"

/**
 * An enumeration of button IDs
 */
typedef enum buttonID : u8 {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_COUNT
} ButtonID;

typedef struct binding {
	SDL_Scancode keyboard;
} Binding;

/**
 * The default bindings associated with each Button ID
 */
constexpr Binding DEFAULT_BINDINGS[BUTTON_COUNT] = {
	[BUTTON_UP]		= { .keyboard = SDL_SCANCODE_UP },
	[BUTTON_DOWN]	= { .keyboard = SDL_SCANCODE_DOWN },
	[BUTTON_LEFT]	= { .keyboard = SDL_SCANCODE_LEFT },
	[BUTTON_RIGHT]	= { .keyboard = SDL_SCANCODE_RIGHT }
};

/**
 * The state of a button
 */
typedef struct buttonState {
	bool pressed;	/**< Whether the button was pressed on this frame */
	bool held;		/**< Whether the button is held down this frame */
	bool released;	/**< Whether the button was released on this frame */
} ButtonState;

/**
 * A virtual representation of a button
 */
typedef struct virtualButton {
	ButtonID id;
	ButtonState state;
	Binding binding;
} VirtualButton;

/**
 * Initializes the input system
 */
void Input_init();

/**
 * Updates the input system
 */
void Input_update();

/**
 * Gets the state of the button with the specified ID
 * @param buttonID The ID of the button to get
 * @return The state of the button
 */
const ButtonState* Input_getButtonState(u8 buttonID);

#endif //ENGINE_CORE_INPUT_H