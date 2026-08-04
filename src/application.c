#include "application.h"

#include <stdlib.h>
#include <dcimgui.h>
#include <dcimgui_impl_sdl3.h>
#include <dcimgui_impl_sdlgpu3.h>

#include "engine/core/input.h"
#include "engine/core/logger.h"
#include "engine/core/resources.h"
#include "engine/core/vtime.h"
#include "engine/core/window.h"
#include "engine/debug/debugPanel.h"
#include "engine/rendering/renderer.h"
#include "phoenix/phoenix.h"

static struct {
    bool setToExit;
} appState;

static void create();
static void destroy();

static void update();
static void tick();
static void render();

static bool shouldTick();

void Application_run() {
    // Initialize the application
    create();

    // Start the application loop
    while (!appState.setToExit) {
        update();
        while (shouldTick()) {
            tick();
        }
        render();
    }

    // Destroy the application
    destroy();

    // Exit the program
    exit(EXIT_SUCCESS);
}

void Application_exit() {
    Logger_info("Application set to exit");
    appState.setToExit = true;
}

void create() {
    // Initialize the application state
    appState = (typeof(appState)){
        .setToExit = false
    };

    // Create the logger
    Logger_create(LOG_LEVEL_DEBUG, APPLICATION_NAME, APPLICATION_VERSION);

    // Initialize SDL
    Logger_info("Initializing SDL...");
    Logger_pushIndent();
    SDL_SetAppMetadata(APPLICATION_NAME, APPLICATION_VERSION, APPLICATION_IDENTIFIER);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Logger_error("Failed to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    Logger_popIndent();

    // Create application systems
    Time_init();
    Window_create(APPLICATION_NAME, Application_exit);
    Renderer_create(Window_getHandle());
    Input_init();

    // Initialize ImGui
    CIMGUI_CHECKVERSION();
    ImGui_CreateContext(nullptr);
    cImGui_ImplSDL3_InitForSDLGPU(Window_getHandle());
    ImGui_ImplSDLGPU3_InitInfo imguiInitInfo = {
        .Device = Renderer_getGPUDevice(),
        .ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(Renderer_getGPUDevice(), Window_getHandle()),
    };
    cImGui_ImplSDLGPU3_Init(&imguiInitInfo);
    ImGuiIO* imguiIO = ImGui_GetIO();
    imguiIO->IniFilename = nullptr;

    // Initialize the debug panel
    DebugPanel_init();

    // Create the application
    Logger_info("Creating %s...", APPLICATION_NAME);
    Logger_pushIndent();
    APPLICATION_CREATE_CB();
    Logger_popIndent();

    // Show the window
    Window_show();
}

void destroy() {
    // Destroy the application
    Logger_info("Destroying %s...", APPLICATION_NAME);
    Logger_pushIndent();
    APPLICATION_DESTROY_CB();
    Logger_popIndent();

    // Destroy ImGui
    SDL_WaitForGPUIdle(Renderer_getGPUDevice());
    cImGui_ImplSDL3_Shutdown();
    cImGui_ImplSDLGPU3_Shutdown();
    ImGui_DestroyContext(nullptr);

    // Destroy application systems
    Renderer_destroy();
    Window_destroy();

    // Quit SDL
    Logger_info("Quitting SDL...");
    Logger_pushIndent();
    SDL_Quit();
    Logger_popIndent();

    // Destroy the log
    Logger_destroy();
}

void update() {
    // Update time
    Time_endProfiler(PROFILER_FRAME_DELTA);
    Time_update();
    Time_startProfiler(PROFILER_FRAME_DELTA);
    Time_startProfiler(PROFILER_UPDATE);

    // Poll SDL events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // Pass the event to ImGui
        cImGui_ImplSDL3_ProcessEvent(&e);

        // Hanlde the event
        switch (e.type) {
            case SDL_EVENT_QUIT:
                Application_exit();
                break;
            default:
                break;
        }
    }

    // Begin the ImGui frame
    cImGui_ImplSDLGPU3_NewFrame();
    cImGui_ImplSDL3_NewFrame();
    ImGui_NewFrame();

    // Update engine subsystems
    Input_update();
    DebugPanel_update();

    // Update the application
    APPLICATION_UPDATE_CB();

    // End update profiling
    Time_endProfiler(PROFILER_UPDATE);
}

void tick() {
    // Start tick profiling
    Time_startProfiler(PROFILER_TICK);

    // Tick the application
    APPLICATION_TICK_CB();

    // End tick profiling
    Time_endProfiler(PROFILER_TICK);
}

void render() {
    // Start render profiling
    Time_startProfiler(PROFILER_RENDER);

    // Begin ImGui rendering
    ImGui_Render();

    // Render the application
    APPLICATION_RENDER_CB();

    // Render the frame
    Renderer_render();

    // End render profiling
    Time_endProfiler(PROFILER_RENDER);
}

bool shouldTick() {
    return Time_consumeTick();
}
