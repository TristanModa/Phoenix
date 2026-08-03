#include "debugPanel.h"

#include <dcimgui.h>
#include <mimalloc-stats.h>

#include "engine/core/input.h"
#include "engine/core/logger.h"
#include "engine/core/vtime.h"

static struct {
    bool enabled;

    struct {

    } log;
} panelState;

static void setStyle();

static void drawGeneralTab();
static void drawPerformanceTab();
static void drawLogTab();

void DebugPanel_init() {
    // Initialize panel state
    panelState = (typeof(panelState)){ .enabled = true };

	// Set the ImGui style
	setStyle();

    // Load the debug panel font
    const ImGuiIO* io = ImGui_GetIO();
    ImFontAtlas_AddFontFromFileTTF(io->Fonts, FONT_PATH, 16.0f, nullptr, ImFontAtlas_GetGlyphRangesDefault(io->Fonts));
}

void DebugPanel_update() {
    // Toggle the debug panel's visibility if the toggle button is pressed
    const bool togglePressed = Input_getButtonState(BUTTON_DEBUG_PANEL)->pressed;
    if (togglePressed) {
        panelState.enabled = !panelState.enabled;
    }

    // Avoid drawing when the panel is inactive
    if (!panelState.enabled) {
        return;
    }

    // ImGui_ShowDemoWindow(nullptr);

    // Draw debug panel
    if (ImGui_Begin("Debug Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui_BeginTabBar("Panel Tab Bar", ImGuiTabBarFlags_None)) {
            // General tab
            if (ImGui_BeginTabItem("General", nullptr, ImGuiTabItemFlags_None)) {
                drawGeneralTab();
                ImGui_EndTabItem();
            }

            // Performance tab
            if (ImGui_BeginTabItem("Performance", nullptr, ImGuiTabItemFlags_None)) {
                drawPerformanceTab();
                ImGui_EndTabItem();
            }

            // Log tab
            if (ImGui_BeginTabItem("Log", nullptr, ImGuiTabItemFlags_None)) {
                drawLogTab();
                ImGui_EndTabItem();
            }

            ImGui_EndTabBar();
        }
    } ImGui_End();
}

void setStyle() {
    // Hazy Dark style by kaitabuchi314 from ImThemes
	ImGuiStyle* style = ImGui_GetStyle();

	style->Alpha = 1.0f;
	style->DisabledAlpha = 0.6f;
	style->WindowPadding = (ImVec2){ .x = 5.5f, .y = 8.3f };
	style->WindowRounding = 4.5f;
	style->WindowBorderSize = 1.0f;
	style->WindowMinSize = (ImVec2){ .x = 32.0f, .y = 32.0f };
	style->WindowTitleAlign = (ImVec2){ .x = 0.0f, .y = 0.5f };
	style->WindowMenuButtonPosition = ImGuiDir_Left;
	style->ChildRounding = 3.2f;
	style->ChildBorderSize = 1.0f;
	style->PopupRounding = 2.7f;
	style->PopupBorderSize = 1.0f;
	style->FramePadding = (ImVec2){ .x = 4.0f, .y = 3.0f };
	style->FrameRounding = 2.4f;
	style->FrameBorderSize = 0.0f;
	style->ItemSpacing = (ImVec2){ .x = 8.0f, .y = 4.0f };
	style->ItemInnerSpacing = (ImVec2){ .x = 4.0f, .y = 4.0f };
	style->CellPadding = (ImVec2){ .x = 4.0f, .y = 2.0f };
	style->IndentSpacing = 21.0f;
	style->ColumnsMinSpacing = 6.0f;
	style->ScrollbarSize = 14.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 10.0f;
	style->GrabRounding = 3.2f;
	style->TabRounding = 3.5f;
	style->TabBorderSize = 1.0f;
	style->ColorButtonPosition = ImGuiDir_Right;
	style->ButtonTextAlign = (ImVec2){ .x = 0.5f, .y = 0.5f };
	style->SelectableTextAlign = (ImVec2){ .x = 0.0f, .y = 0.0f };

	style->Colors[ImGuiCol_Text] = (ImVec4){ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f };
	style->Colors[ImGuiCol_TextDisabled] = (ImVec4){ .x = 0.49803922f, .y = 0.49803922f, .z = 0.49803922f, .w = 1.0f };
	style->Colors[ImGuiCol_WindowBg] = (ImVec4){ .x = 0.05882353f, .y = 0.05882353f, .z = 0.05882353f, .w = 0.94f };
	style->Colors[ImGuiCol_ChildBg] = (ImVec4){ .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };
	style->Colors[ImGuiCol_PopupBg] = (ImVec4){ .x = 0.078431375f, .y = 0.078431375f, .z = 0.078431375f, .w = 0.94f };
	style->Colors[ImGuiCol_Border] = (ImVec4){ .x = 0.42745098f, .y = 0.42745098f, .z = 0.49803922f, .w = 0.5f };
	style->Colors[ImGuiCol_BorderShadow] = (ImVec4){ .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };
	style->Colors[ImGuiCol_FrameBg] = (ImVec4){ .x = 0.13725491f, .y = 0.17254902f, .z = 0.22745098f, .w = 0.54f };
	style->Colors[ImGuiCol_FrameBgHovered] = (ImVec4){ .x = 0.21176471f, .y = 0.25490198f, .z = 0.3019608f, .w = 0.4f };
	style->Colors[ImGuiCol_FrameBgActive] = (ImVec4){ .x = 0.043137256f, .y = 0.047058824f, .z = 0.047058824f, .w = 0.67f };
	style->Colors[ImGuiCol_TitleBg] = (ImVec4){ .x = 0.039215688f, .y = 0.039215688f, .z = 0.039215688f, .w = 1.0f };
	style->Colors[ImGuiCol_TitleBgActive] = (ImVec4){ .x = 0.078431375f, .y = 0.08235294f, .z = 0.09019608f, .w = 1.0f };
	style->Colors[ImGuiCol_TitleBgCollapsed] = (ImVec4){ .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.51f };
	style->Colors[ImGuiCol_MenuBarBg] = (ImVec4){ .x = 0.13725491f, .y = 0.13725491f, .z = 0.13725491f, .w = 1.0f };
	style->Colors[ImGuiCol_ScrollbarBg] = (ImVec4){ .x = 0.019607844f, .y = 0.019607844f, .z = 0.019607844f, .w = 0.53f };
	style->Colors[ImGuiCol_ScrollbarGrab] = (ImVec4){ .x = 0.30980393f, .y = 0.30980393f, .z = 0.30980393f, .w = 1.0f };
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = (ImVec4){ .x = 0.40784314f, .y = 0.40784314f, .z = 0.40784314f, .w = 1.0f };
	style->Colors[ImGuiCol_ScrollbarGrabActive] = (ImVec4){ .x = 0.50980395f, .y = 0.50980395f, .z = 0.50980395f, .w = 1.0f };
	style->Colors[ImGuiCol_CheckMark] = (ImVec4){ .x = 0.7176471f, .y = 0.78431374f, .z = 0.84313726f, .w = 1.0f };
	style->Colors[ImGuiCol_SliderGrab] = (ImVec4){ .x = 0.47843137f, .y = 0.5254902f, .z = 0.57254905f, .w = 1.0f };
	style->Colors[ImGuiCol_SliderGrabActive] = (ImVec4){ .x = 0.2901961f, .y = 0.31764707f, .z = 0.3529412f, .w = 1.0f };
	style->Colors[ImGuiCol_Button] = (ImVec4){ .x = 0.14901961f, .y = 0.16078432f, .z = 0.1764706f, .w = 0.4f };
	style->Colors[ImGuiCol_ButtonHovered] = (ImVec4){ .x = 0.13725491f, .y = 0.14509805f, .z = 0.15686275f, .w = 1.0f };
	style->Colors[ImGuiCol_ButtonActive] = (ImVec4){ .x = 0.078431375f, .y = 0.08627451f, .z = 0.09019608f, .w = 1.0f };
	style->Colors[ImGuiCol_Header] = (ImVec4){ .x = 0.19607843f, .y = 0.21568628f, .z = 0.23921569f, .w = 0.31f };
	style->Colors[ImGuiCol_HeaderHovered] = (ImVec4){ .x = 0.16470589f, .y = 0.1764706f, .z = 0.19215687f, .w = 0.8f };
	style->Colors[ImGuiCol_HeaderActive] = (ImVec4){ .x = 0.07450981f, .y = 0.08235294f, .z = 0.09019608f, .w = 1.0f };
	style->Colors[ImGuiCol_Separator] = (ImVec4){ .x = 0.42745098f, .y = 0.42745098f, .z = 0.49803922f, .w = 0.5f };
	style->Colors[ImGuiCol_SeparatorHovered] = (ImVec4){ .x = 0.23921569f, .y = 0.3254902f, .z = 0.42352942f, .w = 0.78f };
	style->Colors[ImGuiCol_SeparatorActive] = (ImVec4){ .x = 0.27450982f, .y = 0.38039216f, .z = 0.49803922f, .w = 1.0f };
	style->Colors[ImGuiCol_ResizeGrip] = (ImVec4){ .x = 0.2901961f, .y = 0.32941177f, .z = 0.3764706f, .w = 0.2f };
	style->Colors[ImGuiCol_ResizeGripHovered] = (ImVec4){ .x = 0.23921569f, .y = 0.29803923f, .z = 0.36862746f, .w = 0.67f };
	style->Colors[ImGuiCol_ResizeGripActive] = (ImVec4){ .x = 0.16470589f, .y = 0.1764706f, .z = 0.1882353f, .w = 0.95f };
	style->Colors[ImGuiCol_Tab] = (ImVec4){ .x = 0.11764706f, .y = 0.1254902f, .z = 0.13333334f, .w = 0.862f };
	style->Colors[ImGuiCol_TabHovered] = (ImVec4){ .x = 0.32941177f, .y = 0.40784314f, .z = 0.5019608f, .w = 0.8f };
	style->Colors[ImGuiCol_TabActive] = (ImVec4){ .x = 0.24313726f, .y = 0.24705882f, .z = 0.25490198f, .w = 1.0f };
	style->Colors[ImGuiCol_TabUnfocused] = (ImVec4){ .x = 0.06666667f, .y = 0.101960786f, .z = 0.14509805f, .w = 0.9724f };
	style->Colors[ImGuiCol_TabUnfocusedActive] = (ImVec4){ .x = 0.13333334f, .y = 0.25882354f, .z = 0.42352942f, .w = 1.0f };
	style->Colors[ImGuiCol_PlotLines] = (ImVec4){ .x = 0.60784316f, .y = 0.60784316f, .z = 0.60784316f, .w = 1.0f };
	style->Colors[ImGuiCol_PlotLinesHovered] = (ImVec4){ .x = 1.0f, .y = 0.42745098f, .z = 0.34901962f, .w = 1.0f };
	style->Colors[ImGuiCol_PlotHistogram] = (ImVec4){ .x = 0.8980392f, .y = 0.69803923f, .z = 0.0f, .w = 1.0f };
	style->Colors[ImGuiCol_PlotHistogramHovered] = (ImVec4){ .x = 1.0f, .y = 0.6f, .z = 0.0f, .w = 1.0f };
	style->Colors[ImGuiCol_TableHeaderBg] = (ImVec4){ .x = 0.1882353f, .y = 0.1882353f, .z = 0.2f, .w = 1.0f };
	style->Colors[ImGuiCol_TableBorderStrong] = (ImVec4){ .x = 0.30980393f, .y = 0.30980393f, .z = 0.34901962f, .w = 1.0f };
	style->Colors[ImGuiCol_TableBorderLight] = (ImVec4){ .x = 0.22745098f, .y = 0.22745098f, .z = 0.24705882f, .w = 1.0f };
	style->Colors[ImGuiCol_TableRowBg] = (ImVec4){ .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };
	style->Colors[ImGuiCol_TableRowBgAlt] = (ImVec4){ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 0.06f };
	style->Colors[ImGuiCol_TextSelectedBg] = (ImVec4){ .x = 0.25882354f, .y = 0.5882353f, .z = 0.9764706f, .w = 0.35f };
	style->Colors[ImGuiCol_DragDropTarget] = (ImVec4){ .x = 1.0f, .y = 1.0f, .z = 0.0f, .w = 0.9f };
	style->Colors[ImGuiCol_NavHighlight] = (ImVec4){ .x = 0.25882354f, .y = 0.5882353f, .z = 0.9764706f, .w = 1.0f };
	style->Colors[ImGuiCol_NavWindowingHighlight] = (ImVec4){ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 0.7f };
	style->Colors[ImGuiCol_NavWindowingDimBg] = (ImVec4){ .x = 0.8f, .y = 0.8f, .z = 0.8f, .w = 0.2f };
	style->Colors[ImGuiCol_ModalWindowDimBg] = (ImVec4){ .x = 0.8f, .y = 0.8f, .z = 0.8f, .w = 0.35f };
}

void drawGeneralTab() {

}

void drawPerformanceTab() {
	ImGui_SeparatorText("Time");
	ImGui_Text("FPS: %f", 1.0f / Time_getDeltaTime());
}

void drawLogTab() {

	ImGui_Separator();



}