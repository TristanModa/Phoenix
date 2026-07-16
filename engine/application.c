#include "application.h"

#include <stdio.h>
#include <stdlib.h>

#include "gfx/gfx.h"

static ApplicationState appState;

static void init();
static void destroy();
static void update();
static void tick();
static void render();

static void onWindowClose();

void Application_create(const char *name, const AppVersion version, const AppCallback initCallback,
    const AppCallback destroyCallback, const AppCallback updateCallback, const AppCallback tickCallback,
    const AppCallback renderCallback) {
    // Initialize application state
    appState.name = name;
    appState.version = version;

    appState.initCallback = initCallback;
    appState.destroyCallback = destroyCallback;
    appState.updateCallback = updateCallback;
    appState.tickCallback = tickCallback;
    appState.renderCallback = renderCallback;

    appState.setToExit = false;

    // Construct the version string
    snprintf(appState.versionString, VERSION_STRING_MAX_LEN, "v%u.%u.%u",
        appState.version.major,
        appState.version.minor,
        appState.version.patch);

    // Set the application's default allocators
    Memory_setAllocators(malloc, calloc, realloc, free);
}

void Application_run() {
    // Initialize the application
    init();

    // Run the application loop
    while (!Application_shouldExit()) {
        // Update
        update();

        // Tick
        while (Time_consumeTick()) {
            tick();
        }

        // Redner
        render();
    }

    // Destroy the application
    destroy();
}

void Application_exit(const int code) {
    appState.setToExit = true;
    appState.exitCode = code;
    Logger_info("Application set to exit with code %d.", appState.exitCode);
}

void Application_exitImmediate(const int code) {
    appState.exitCode = code;
    Logger_info("Application exited with code %d.", appState.exitCode);
    Logger_closeLog();
    exit(appState.exitCode);
}

const char* Application_getName() {
    return appState.name;
}

const AppVersion* Application_getVersion() {
    return &appState.version;
}

const char* Application_getVersionString() {
    return appState.versionString;
}

bool Application_shouldExit() {
    return appState.setToExit;
}

void init() {
    // Initialize the logger
    Logger_init(LOGGER_LOG_LEVEL_DEBUG, appState.name, appState.versionString);

    // Initialize the memory allocator
    Memory_init();

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Logger_fatal("Failed to initialize SDL: %s", SDL_GetError());
        Application_exitImmediate(-1);
    }

    // Initialize application subsystems
    Time_init();
    if (!Window_create(appState.name, onWindowClose)) {
        Logger_fatal("Failed to create the application window.");
        Application_exitImmediate(-1);
    }
    if (!Renderer_create()) {
        Logger_fatal("Failed to create the application renderer.");
        Application_exitImmediate(-1);
    }

    // Show the application window
    Window_show();

    // Call the init callback
    appState.initCallback();

    Logger_info("Application initialized.");
}

void destroy() {
    Logger_info("Shutting down application...");

    // Call the destroy callback
    appState.destroyCallback();

    // Destroy application subsystems
    Renderer_destroy();
    Window_destroy();

    // Quit SDL
    SDL_Quit();

    // Exit the application
    Logger_info("Application exited with code %d.", appState.exitCode);
    Logger_closeLog();
    exit(appState.exitCode);
}

void update() {
    // Poll window events
    Window_pollEvents();

    // Update application subsystems
    Time_update();

    // Call the update callback
    appState.updateCallback();
}

void tick() {
    // Call the tick callback
    appState.tickCallback();
}

void render() {
    // Call the render callback
    appState.renderCallback();
}

void onWindowClose() {
    // Exit with code zero
    Application_exit(0);
}