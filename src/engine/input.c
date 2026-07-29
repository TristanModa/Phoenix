#include "input.h"

#include "logger.h"

static struct {
    VirtualButton buttons[BUTTON_COUNT];
} inputState;

static void updateButton(VirtualButton* button);

void Input_init() {
    // Populate the button array
    for (ButtonID i = 0; i < BUTTON_COUNT; i++) {
        inputState.buttons[i] = (VirtualButton){
            .state = (ButtonState){},
            .binding = DEFAULT_BINDINGS[i]
        };
    }
}

void Input_update() {
    // Update each button
    for (int i = 0; i < BUTTON_COUNT; i++) {
        updateButton(&inputState.buttons[i]);
    }
}

const ButtonState* Input_getButtonState(const u8 buttonID) {
    // Return a default state if no button of the specified id exists
    if (buttonID >= BUTTON_COUNT) {
        Logger_error("Failed to get button state: Button with ID of %hhu does not exist.", buttonID);
        static constexpr ButtonState FALLBACK_BUTTON_STATE = {};
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