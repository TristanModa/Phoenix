#include "application.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/core.h"
#include "gfx/gfx.h"

static ApplicationState appState;

static void setDefaultSubsystemProperties();

static void init();
static void destroy();
static void update();
static void tick();
static void render();

void Application_create(const char* name, const char* identifier, const AppVersion version,
    const AppCallback initCallback, const AppCallback destroyCallback, const AppCallback updateCallback,
    const AppCallback tickCallback, const AppCallback renderCallback) {
    // Initialize application state
    appState.name = name;
    appState.identifier = identifier;
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

    // Set the default properties of all app subsystems
    setDefaultSubsystemProperties();
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

        // Render
        render();
    }

    // Destroy the application
    destroy();
    exit(EXIT_SUCCESS);
}

void Application_exit() {
    appState.setToExit = true;
    Logger_info("Application set to exit.");
}

void Application_exitImmediate() {
    Logger_info("Application set to exit immediately.");
    Logger_closeLog();
    exit(EXIT_SUCCESS);
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

void setDefaultSubsystemProperties() {
    // Set the default logger properties
    const LoggerProperties loggerProperties = {
        .logFilePath = "app.log"
    };
    Logger_setProperties(&loggerProperties);

    // Set the default time properties
    const TimeProperties timeProperties = {
        .targetTicksPerSecond = 50,
        .maxTickProcessingTimePerFrame = 0.25f
    };
    Time_setProperties(&timeProperties);

    // Set the default window properties
    const WindowProperties windowProperties = {
        .flags = 0,
        .width = 1280,
        .height = 720
    };
    Window_setProperties(&windowProperties);

    // Set the default input properties
    const InputProperties inputProperties = {
        .buttonCount = 0,
        .defaultBindings = nullptr
    };
    Input_setProperties(&inputProperties);
}

void init() {
    // Initialize the signal handler
    SignalHandler_init();

    // Initialize the logger
    Logger_init(LOGGER_LOG_LEVEL_DEBUG, appState.name, appState.versionString);

    // Log missing application callbacks
    VoidFn appCallbacks[] = {
        appState.initCallback, appState.destroyCallback,
        appState.updateCallback, appState.tickCallback, appState.renderCallback
    };
    const char* appCallbackNames[] = {
        "init", "destroy", "update", "tick", "render"
    };
    for (size_t i = 0; i < sizeof(appCallbacks) / sizeof(appCallbacks[0]); i++) {
        Logger_warning("Application %s callback is null", appCallbackNames[i]);
    }

    // Initialize SDL
    Logger_info("Initializing SDL...");
    Logger_pushIndent();
    SDL_SetMemoryFunctions(Memory_malloc, Memory_calloc, Memory_realloc, Memory_free);
    SDL_SetAppMetadata(appState.name, appState.versionString, appState.identifier);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Logger_fatal("Failed to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    Logger_popIndent();

    // Initialize application subsystems
    Time_init();
    Window_create(appState.name, Application_exit);
    Renderer_create();
    Input_create();

    // Show the application window
    Window_show();

    // Call the init callback
    Logger_info("Initializing %s...", appState.name);
    Logger_pushIndent();
    if (appState.initCallback) appState.initCallback();

    Logger_popIndent();
}

void destroy() {
    // Call the destroy callback
    Logger_info("Destroying %s...", appState.name);
    Logger_pushIndent();
    if (appState.destroyCallback) appState.destroyCallback();
    Logger_popIndent();

    // Destroy application subsystems
    Input_destroy();
    Renderer_destroy();
    Window_destroy();

    // Quit SDL
    Logger_info("Quitting SDL...");
    Logger_pushIndent();
    SDL_Quit();
    Logger_popIndent();

    // Exit the application
    Logger_info("Application shut down successfully");
    Logger_closeLog();
}

void update() {
    // Poll window events
    Window_pollEvents();

    // Update application subsystems
    Time_update();
    Input_update();

    // Call the update callback
    if (appState.updateCallback) appState.updateCallback();
}

void tick() {
    // Call the tick callback
    if (appState.tickCallback) appState.tickCallback();
}

void render() {
    // Call the render callback
    if (appState.renderCallback) appState.renderCallback();

    // Render the frame
    Renderer_render();
}