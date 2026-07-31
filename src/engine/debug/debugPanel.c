#include "debugPanel.h"

#include <dcimgui.h>

#include "engine/core/input.h"
#include "engine/core/vtime.h"

static struct {
    bool enabled;
} panelState;

void DebugPanel_init() {
    panelState = (typeof(panelState)){};
}

void DebugPanel_update() {
    // Toggle the debug panel's visibility if the toggle button is pressed
    bool togglePressed = Input_getButtonState(BUTTON_DEBUG_PANEL)->pressed;
    if (togglePressed) {
        panelState.enabled = !panelState.enabled;
    }

    // Avoid drawing when the panel is inactive
    if (!panelState.enabled) {
        return;
    }

    // Draw windows
    if (ImGui_Begin("Debug panel", nullptr, ImGuiWindowFlags_None)) {
        ImGui_Text("FPS: %.2f", 1.0f / Time_getDeltaTime());
        ImGui_Text("Current time: %.2f", Time_getCurrentTime());
        ImGui_Text("Current tick: %d", Time_getCurrentTick());
    } ImGui_End();
}
