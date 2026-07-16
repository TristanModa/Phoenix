#ifndef ENGINE_CORE_INPUT_H
#define ENGINE_CORE_INPUT_H

#include <SDL3/SDL.h>

#include "types.h"

typedef struct binding {
    SDL_Scancode keyboard;
} Binding;

/**
 * Defines a configuration for the input subsystem
 */
typedef struct inputProperties {
    /**
     * The amount of virtual buttons that should be supported
     */
    int buttonCount;
    /**
     * A pointer to an array of default bindings for each button id.
     * Should have length equal to or greater than button count.
     */
    const Binding* defaultBindings;
} InputProperties;

/**
 *
 */
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

/**
 * Sets the properties of the input subsystem.
 * Should only be called prior to running the application via Application_run.
 * @param inputProperties The properties struct to use
 */
void Input_setProperties(const InputProperties* inputProperties);

/**
 * Creates the input subsystem
 */
void Input_create();
/**
 * Destroys the input subsystem and frees associated resources
 */
void Input_destroy();

void Input_update();

const ButtonState* Input_getButtonState(u8 buttonID);

#endif //ENGINE_CORE_INPUT_H
