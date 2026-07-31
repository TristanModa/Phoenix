#ifndef APPLICATION_H
#define APPLICATION_H

#include "engine/core/types.h"
#include "phoenix/phoenix.h"

/**
 * The name of the application
 */
constexpr char APPLICATION_NAME[] = "Phoenix";
/**
 * The identifier of the application
 */
constexpr char APPLICATION_IDENTIFIER[] = "com.vulf.phoenix";
/**
 * The version of the application
 */
constexpr char APPLICATION_VERSION[] = "v" VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH;

/**
 * Callback for creating the application
 */
#define APPLICATION_CREATE_CB Phoenix_create
/**
 * Callback for destroying the application
 */
#define APPLICATION_DESTROY_CB Phoenix_destroy
/**
 * Callback for updating the application
 */
#define APPLICATION_UPDATE_CB Phoenix_update
/**
 * Callback for ticking the application
 */
#define APPLICATION_TICK_CB Phoenix_tick
/**
 * Callback for rendering current frame of the application
 */
#define APPLICATION_RENDER_CB Phoenix_render

/**
 * Initializes the application and starts the application loop.
 * This function will never return as it terminates the process on application shutdown.
 */
void Application_run();
/**
 * Sets the application to exit at the end of the next frame
 */
void Application_exit();

#endif //APPLICATION_H