#include "input.h"

#include "logger.h"
#include "memory.h"

static InputState inputState;

static void updateButton(VirtualButton* button);

void Input_setProperties(const InputProperties* inputProperties) {
    inputState.buttonCount = inputProperties->buttonCount;
    inputState.defaultBindings = inputProperties->defaultBindings;
}

void Input_create() {
    Logger_info("Creating input subsystem...");
    Logger_pushIndent();

    // Allocate memory for the button array
    inputState.buttons = Memory_malloc(inputState.buttonCount * sizeof(VirtualButton));
    if (!inputState.buttons) {
        Logger_fatal("Failed to create input subsystem: Memory allocation failed.");
        exit(-1);
    }

    // Populate the button array
    for (int i = 0; i < inputState.buttonCount; i++) {
        VirtualButton* button = &inputState.buttons[i];
        button->id = i;
        button->state = (ButtonState){};
        button->binding = inputState.defaultBindings[i];
    }

    // Log a warning if no buttons were defined
    if (inputState.buttonCount == 0) {
        Logger_warning("No buttons defined. No input will be read by the application.");
    }

    // Pop the log indent
    Logger_popIndent();
}

void Input_destroy() {
    Logger_info("Destroying input subsystem...");

    // Release associated resources
    Memory_free(inputState.buttons);
}

void Input_update() {
    // Update each button
    for (int i = 0; i < inputState.buttonCount; i++) {
        updateButton(&inputState.buttons[i]);
    }
}

const ButtonState* Input_getButtonState(const u8 buttonID) {
    // Return a
    if (buttonID >= inputState.buttonCount) {
        Logger_error("Failed to get button state: Button with ID of %hhu does not exist.", buttonID);
        static constexpr ButtonState FALLBACK_BUTTON_STATE = { false, false, false };
        return &FALLBACK_BUTTON_STATE;
    }

    // Get the button state
    return &inputState.buttons[buttonID].state;
}

void updateButton(VirtualButton* button) {
    // Get the button state
    const bool keyDown = SDL_GetKeyboardState(nullptr)[button->binding.keyboard];
    const bool buttonDown = keyDown;

    // Reset pressed and released
    button->state.pressed = false;
    button->state.released = false;

    // If the button is down but was not held last frame it must have been pressed this frame
    if (buttonDown && !button->state.held) {
        button->state.pressed = true;
        button->state.held = true;
    }
    // If the button is up but was held last frame it must have been released this frame
    else if (!buttonDown && button->state.held) {
        button->state.released = true;
        button->state.held = false;
    }
}