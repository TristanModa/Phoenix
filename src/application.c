#include "application.h"

#include <stdlib.h>

#include "engine/input.h"
#include "engine/logger.h"
#include "engine/resources.h"
#include "engine/vtime.h"
#include "engine/window.h"
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
    Logger_create(LOGGER_LOG_LEVEL_DEBUG, APPLICATION_NAME, APPLICATION_VERSION);

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
    Resources_create();

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

    // Destroy application systems
    Resources_destroy();
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
    // Poll window events
    Window_pollEvents();

    // Update application subsystems
    Time_update();
    Input_update();
    Resources_update();

    // Update the application
    APPLICATION_UPDATE_CB();
}

void tick() {
    // Tick the application
    APPLICATION_TICK_CB();
}

void render() {
    // Render the application
    APPLICATION_RENDER_CB();
}

bool shouldTick() {
    return Time_consumeTick();
}
