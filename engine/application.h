#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "core/core.h"

/**
 * The buffer size to use for the version string
 */
constexpr int VERSION_STRING_MAX_LEN = 16;

/**
 * Function pointer for application callbacks
 */
typedef VoidFunc AppCallback;

/**
 * The version of the application
 */
typedef struct appVersion {
    /**
     * The major version of the application
     */
    u8 major;
    /**
     * The minor version of the application
     */
    u8 minor;
    /**
     * The patch number of the application
     */
    u8 patch;
} AppVersion;

/**
 * The state of the application
 */
typedef struct applicationState {
    /**
     * The name of the application
     */
    const char* name;
    /**
     * The version of the application
     */
    AppVersion version;
    /**
     * The string representation of the version of the application
     */
    char versionString[VERSION_STRING_MAX_LEN];

    /**
     * The application callback that will be called on application initialization
     */
    AppCallback initCallback;
    /**
     * The application callback that will be called on application destruction
     */
    AppCallback destroyCallback;
    /**
     * The application callback that will be called on application update
     */
    AppCallback updateCallback;
    /**
     * The application callback that will be called on application initialization
     */
    AppCallback tickCallback;
    /**
     * The application callback that will be called on application render
     */
    AppCallback renderCallback;

    /**
     * Whether the application should exit at the end of the current frame
     */
    bool setToExit;
} ApplicationState;

/**
 * Creates the application
 * @param name The name of the application
 * @param version The version of the application
 * @param initCallback The application callback that will be called on initialization
 * @param destroyCallback The application callback that will be called on destruction
 * @param updateCallback The application callback that will be called on update
 * @param tickCallback The application callback that will be called on tick
 * @param renderCallback The application callback that will be called on render
 */
void Application_create(const char* name, AppVersion version, AppCallback initCallback, AppCallback destroyCallback,
                        AppCallback updateCallback, AppCallback tickCallback, AppCallback renderCallback);

/**
 * Initializes the application and starts the application loop.
 * This function will never return as it terminates the process on application shutdown.
 */
void Application_run();
/**
 * Sets the application to exit at the end of the next frame
 */
void Application_exit();
/**
 * Exits the application immediately.
 * Exiting immediately will avoid running any application destructors and can cause loss of data.
 */
void Application_exitImmediate();

/**
 * Gets the name of the application
 * @return The name as a C string
 */
const char* Application_getName();
/**
 * Gets the version of the application
 * @return The version as a const reference
 */
const AppVersion* Application_getVersion();
/**
 * Gets the string representation of the version of the application
 * @return The version as a C string
 */
const char* Application_getVersionString();
/**
 * Gets whether the application is set to exit at the end of the frame
 * @return True if it is
 */
bool Application_shouldExit();

#endif //ENGINE_APPLICATION_H