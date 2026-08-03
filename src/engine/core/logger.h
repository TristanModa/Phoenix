#ifndef ENGINE_CORE_LOGGER_H
#define ENGINE_CORE_LOGGER_H

#include <stddef.h>

#include "types.h"

/**
 * The path that log output should be written to
 */
constexpr char LOG_PATH[] = "phoenix.log";
/**
 * The maximum length a log message can have
 */
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 256;
/**
 * The size of the log history buffer in bytes
 */
constexpr size_t LOG_HISTORY_BUFFER_SIZE = 32768;

static_assert(MAX_LOG_MESSAGE_LENGTH < LOG_HISTORY_BUFFER_SIZE);
static_assert((LOG_HISTORY_BUFFER_SIZE - 1 & LOG_HISTORY_BUFFER_SIZE) == 0);

/**
 * Log sink IDs for various log outputs
 */
typedef enum logSinkID : u8 {
    LOG_SINK_STDOUT,    /**< Log sink for writing to stdout */
    LOG_SINK_LOG_FILE,  /**< Log sink for writing to a log file */
    LOG_SINK_GUI_LOG,   /**< Log sink for writing to the application's debug GUI */
    LOG_SINK_COUNT      /**< The number of log sinks the logger supports */
} LogSinkID;

/**
 * Severity levels for log messages
 */
typedef enum logLevel : u8 {
    LOG_LEVEL_UNKNOWN,  /**< Indicates an undefined or uninitialized log level */
    LOG_LEVEL_DEBUG,    /**< Indicates diagnostic information used for debugging purposes */
    LOG_LEVEL_INFO,     /**< Indicates information on expected system behavior */
    LOG_LEVEL_WARNING,  /**< Indicates abnormalities that do not impact execution */
    LOG_LEVEL_ERROR,    /**< Indicates failures that impact functionality but will not result in application termination */
    LOG_LEVEL_FATAL,    /**< Indicates an unrecoverable failure that will result in application termination */
} LogLevel;

/**
 * Creates the logger
 * @param logLevel The minimum log level a message must have to be written to the log
 * @param appName The name of the application the log is for
 * @param appVersion The version of the application the log is for
 */
void Logger_create(LogLevel logLevel, const char* appName, const char* appVersion);
/**
 * Closes the log file
 */
void Logger_destroy();

/**
 * Writes a formatted message to all log streams
 * @param logLevel The log level of the message
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_log(LogLevel logLevel, const char* format, ...)  __attribute__((format(printf, 2, 3)));
/**
 * Writes a formatted debug message to all log streams
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_debug(const char* format, ...) __attribute__((format(printf, 1, 2)));
/**
 * Writes a formatted info message to all log streams
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_info(const char* format, ...) __attribute__((format(printf, 1, 2)));
/**
 * Writes a formatted warning message to all log streams
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_warning(const char* format, ...) __attribute__((format(printf, 1, 2)));
/**
 * Writes a formatted error message to all log streams
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_error(const char* format, ...) __attribute__((format(printf, 1, 2)));
/**
 * Writes a formatted fatal error message to all log streams
 * @param format The format string of the message
 * @param ... The arguments of the format string
 */
void Logger_fatal(const char* format, ...) __attribute__((format(printf, 1, 2)));

/**
 * Increases the amount of indentation after the log message label by one
 */
void Logger_pushIndent();
/**
 * Decreases the amount of indentation after the log message label by one
 */
void Logger_popIndent();

/**
 * Gets the minimum log level of the logger
 * @return The minimum log level
 */
LogLevel Logger_getLogLevel();
/**
 * Sets the minimum log level of the logger
 * @param logLevel The minimum log level
 */
void Logger_setLogLevel(LogLevel logLevel);

/**
 * Gets the string representation of a log level
 * @param logLevel The log level to get the string representation of
 * @return The log level as a C string
 */
const char* Logger_getLogLevelString(LogLevel logLevel);

void Logger_beginTraverseHistoryBuffer();
const char* Logger_getNextHistoryLine(size_t* n);

#endif //ENGINE_CORE_LOGGER_H
