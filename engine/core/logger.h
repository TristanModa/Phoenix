#ifndef ENGINE_CORE_LOGGER_H
#define ENGINE_CORE_LOGGER_H

#include <stdio.h>

#include "types.h"

#define ANSI_RESET "\x1B[39m"
#define ANSI_DEBUG "\x1B[38;5;14m"
#define ANSI_INFO "\x1B[38;5;15m"
#define ANSI_WARNING "\x1B[38;5;11m"
#define ANSI_ERROR "\x1B[38;5;1m"
#define ANSI_FATAL "\x1B[38;5;88m"
#define ANSI_UNKNOWN "\x1B[38;5;7m"

/**
 * Defines a configuration for the logger subsystem
 */
typedef struct loggerProperties {
    /**
     * The file path to write the log file to
     */
    const char* logFilePath;
} LoggerProperties;

/**
 * Output stream types supported by the logger
 */
typedef enum logOutputStream {
    LOGGER_OUTPUT_STREAM_STDOUT, /**< Log stream for C standard output */
    LOGGER_OUTPUT_STREAM_LOG_FILE, /**< Log stream for logging to file */
    LOGGER_OUTPUT_STREAM_COUNT /**< The amount of supported log streams */
} LogOutputStream;

/**
 * Severity levels for log messages
 */
typedef enum logLevel {
    LOGGER_LOG_LEVEL_DEBUG, /**< Indicates diagnostic information used for debugging purposes */
    LOGGER_LOG_LEVEL_INFO, /**< Indicates information on expected system behavior */
    LOGGER_LOG_LEVEL_WARNING, /**< Indicates abnormalities that do not impact execution */
    LOGGER_LOG_LEVEL_ERROR, /**< Indicates failures that impact functionality but will not result in application termination */
    LOGGER_LOG_LEVEL_FATAL /**< Indicates an unrecoverable failure that will result in application termination */
} LogLevel;

/**
 * The state of the logger subsystem
 */
typedef struct loggerState {
    /**
     * A list of all output streams the logger should write to
     */
    FILE* outputStreams[LOGGER_OUTPUT_STREAM_COUNT];
    /**
     * The file path to write the log file to
     */
    const char* logFilePath;
    /**
     * The minimum log level a log message must have to be written to log
     */
    LogLevel logLevel;
    /**
     * The amount of indents to include after the message label
     */
    u32 indentationLevel;
} LoggerState;

/**
 * Sets the properties of the logger subsystem.
 * Should only be called prior to running the application via Application_run.
 * @param loggerProperties The properties struct to use
 */
void Logger_setProperties(const LoggerProperties* loggerProperties);

/**
 * Initializes the logger
 * @param logLevel The minimum log level a message must have to be written to the log
 * @param appName The name of the application the log is for
 * @param appVersion The version of the application the log is for
 */
void Logger_init(LogLevel logLevel, const char* appName, const char* appVersion);
/**
 * Flushes all log streams and closes the log file stream
 */
void Logger_closeLog();

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
 * Gets the log timestamp for the amount of time elapsed since the application was initialized
 * @param buffer The buffer the log timestamp string should be written to
 * @param bufferSize The size of the buffer the log timestamp string should be written to
 */
void Logger_getTimestampString(char* buffer, size_t bufferSize);
/**
 * Gets the string representation of a log level
 * @param logLevel The log level to get the string representation of
 * @return The log level as a C string
 */
const char* Logger_getLogLevelString(LogLevel logLevel);

#endif //ENGINE_CORE_LOGGER_H