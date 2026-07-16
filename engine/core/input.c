#include "input.h"

#include "logger.h"
#include "memory.h"

static InputState inputState;

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