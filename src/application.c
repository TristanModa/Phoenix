#include "application.h"

#include <stdlib.h>

static struct {
    bool setToExit;
} appState;

static void init();
static void destroy();
static void update();
static void render();

void Application_run() {
    // Initialize the application
    init();

    // Start the application loop
    while (!appState.setToExit) {
        update();
        render();
    }

    // Destroy the application
    destroy();

    // Exit the program
    exit(EXIT_SUCCESS);
}

void Application_exit() {
    appState.setToExit = true;
}

static void init() {
    // Initialize the application state
    appState = (typeof(appState)){
        .setToExit = false
    };
}

static void destroy() {

}

static void update() {

}

static void render() {

}