#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include <SDL3/SDL.h>

#include "types.h"

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

constexpr Binding DEFAULT_BINDINGS[BUTTON_COUNT] = {
	[BUTTON_UP]		= { .keyboard = SDL_SCANCODE_UP },
	[BUTTON_DOWN]	= { .keyboard = SDL_SCANCODE_DOWN },
	[BUTTON_LEFT]	= { .keyboard = SDL_SCANCODE_LEFT },
	[BUTTON_RIGHT]	= { .keyboard = SDL_SCANCODE_RIGHT }
};

typedef struct buttonState {
	bool pressed;
	bool held;
	bool released;
} ButtonState;

typedef struct virtualButton {
	ButtonID id;
	ButtonState state;
	Binding binding;
} VirtualButton;

void Input_init();

void Input_update();

const ButtonState* Input_getButtonState(u8 buttonID);

#endif //ENGINE_INPUT_H