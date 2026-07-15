#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "core/core.h"

constexpr int VERSION_STRING_MAX_LEN = 16;

typedef VoidCallback AppCallback;

typedef struct appVersion {
    u8 major;
    u8 minor;
    u8 patch;
} AppVersion;

typedef struct applicationState {
    const char* name;
    AppVersion version;
    char versionString[VERSION_STRING_MAX_LEN];

    AppCallback initCallback;
    AppCallback destroyCallback;
    AppCallback updateCallback;
    AppCallback renderCallback;

    bool setToExit;
    int exitCode;
} ApplicationState;

void Application_create(const char* name, AppVersion version, AppCallback initCallback, AppCallback destroyCallback,
    AppCallback updateCallback, AppCallback renderCallback);

void Application_run();
void Application_exit(int code);
void Application_exitImmediate(int code);

const char* Application_getName();
const AppVersion* Application_getVersion();
const char* Application_getVersionString();
bool Application_shouldExit();

#endif //ENGINE_APPLICATION_H