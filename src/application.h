#ifndef APPLICATION_H
#define APPLICATION_H

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
 * Initializes the application and starts the application loop.
 * This function will never return as it terminates the process on application shutdown.
 */
void Application_run();
/**
 * Sets the application to exit at the end of the next frame
 */
void Application_exit();

#endif //APPLICATION_H