#include "application.h"

#include <stdio.h>
#include <stdlib.h>

#include "gfx/gfx.h"

static ApplicationState appState;

static void setDefaultSubsystemProperties();

static void init();
static void destroy();
static void update();
static void tick();
static void render();

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

    // Set the default memory properties
    const MemoryProperties memoryProperties = {
        .malloc = malloc,
        .calloc = calloc,
        .realloc = realloc,
        .free = free,
        .compare = memcmp,
        .copy = memcpy,
        .move = memmove
    };
    Memory_setProperties(&memoryProperties);

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
    // Initialize the logger
    Logger_init(LOGGER_LOG_LEVEL_DEBUG, appState.name, appState.versionString);

    // Initialize the memory allocator
    Memory_init();

    // Initialize SDL
    Logger_info("Initializing SDL...");
    Logger_pushIndent();
    SDL_SetAppMetadata(appState.name, appState.versionString, nullptr);
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
    appState.initCallback();

    Logger_info("Application initialized.");
}

void destroy() {
    Logger_info("Shutting down application...");

    // Call the destroy callback
    appState.destroyCallback();

    // Destroy application subsystems
    Input_destroy();
    Renderer_destroy();
    Window_destroy();

    // Quit SDL
    Logger_info("Quitting SDL...");
    SDL_Quit();

    // Exit the application
    Logger_info("Application shut down successfully.");
    Logger_closeLog();
}

void update() {
    // Poll window events
    Window_pollEvents();

    // Update application subsystems
    Time_update();
    Input_update();

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

    // Render the frame
    Renderer_render();
}