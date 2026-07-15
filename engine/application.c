#include "application.h"

#include <stdio.h>
#include <stdlib.h>

static ApplicationState appState;

static void init();
static void destroy();
static void update();
static void render();

void Application_create(const char *title, const AppVersion version, const AppCallback initCallback,
    const AppCallback destroyCallback, const AppCallback updateCallback, const AppCallback renderCallback) {
    // Initialize application state
    appState.title = title;
    appState.initCallback = initCallback;
    appState.destroyCallback = destroyCallback;
    appState.updateCallback = updateCallback;
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
        update();
        render();
    }

    // Destroy the application
    destroy();
}

void Application_exit() {
    appState.setToExit = true;
}

const char* Application_getTitle() {
    return appState.title;
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
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        exit(-1);
    }

    // Initialize application systems
    Window_create(appState.title, Application_exit);

    // Show the application window
    Window_show();

    // Call the init callback
    appState.initCallback();
}

void destroy() {
    // Call the destroy callback
    appState.destroyCallback();

    // Destroy application systems
    Window_destroy();

    // Quit SDL
    SDL_Quit();
}

void update() {
    // Poll window events
    Window_pollEvents();

    // Call the update callback
    appState.updateCallback();
}

void render() {
    // Call the render callback
    appState.renderCallback();
}