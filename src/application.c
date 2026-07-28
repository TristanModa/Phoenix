#include "application.h"

#include <stdlib.h>

#include "engine/input.h"
#include "engine/logger.h"
#include "engine/vtime.h"
#include "engine/window.h"

static struct {
    bool setToExit;
} appState;

static void init();
static void destroy();
static void update();
static void tick();
static void render();

void Application_run() {
    // Initialize the application
    init();

    // Start the application loop
    while (!appState.setToExit) {
        update();
        while (Time_consumeTick()) {
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

static void init() {
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

    // Create application subsystems
    Time_init();
    Window_create(APPLICATION_NAME, Application_exit);

    // Show the window
    Window_show();
}

static void destroy() {
    // Destroy application subsystems
    Window_destroy();

    // Quit SDL
    Logger_info("Quitting SDL...");
    Logger_pushIndent();
    SDL_Quit();
    Logger_popIndent();

    // Destroy the log
    Logger_destroy();
}

static void update() {
    // Poll window events
    Window_pollEvents();

    // Update application subsystems
    Time_update();
    Input_update();
}

void tick() {

}

static void render() {

}
