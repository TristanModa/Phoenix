#ifndef ENGINE_CORE_INPUT_H
#define ENGINE_CORE_INPUT_H

#include <SDL3/SDL.h>

#include "types.h"

typedef struct binding {
    SDL_Scancode keyboard;
} Binding;

typedef struct inputProperties {
    int buttonCount;
    const Binding* defaultBindings;
} InputProperties;

typedef struct buttonState {
    bool pressed;
    bool held;
    bool released;
} ButtonState;

typedef struct virtualButton {
    u8 id;
    ButtonState state;
    Binding binding;
} VirtualButton;

typedef struct inputState {
    int buttonCount;
    const Binding* defaultBindings;

    VirtualButton* buttons;
} InputState;

void Input_setProperties(const InputProperties* inputProperties);

void Input_create();
void Input_destroy();

#endif //ENGINE_CORE_INPUT_H
