#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <SDL3/SDL.h>

#include "types.h"

static LoggerState loggerState;

void Logger_init(const LogLevel logLevel, const char* appName, const char* appVersion) {
    // Set the log level
    loggerState.logLevel = logLevel;

    // Set the logger outputs
    loggerState.outputStreams[LOGGER_OUTPUT_STREAM_STDOUT] = stdout;
    loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE] = fopen("app.log", "w");

    // Print the log header to each output stream
    for (int i = 0; i < LOGGER_OUTPUT_STREAM_COUNT; i++) {
        // Skip this stream if it is null
        if (!loggerState.outputStreams[i]) {
            continue;
        }

        // Print the log header
        fprintf(loggerState.outputStreams[i],
            "%s Log %s\n",
            appName,
            appVersion);
    }

    // Output log initialized
    Logger_info("Logger initialized.");
    if (!loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE]) {
        Logger_error("Unable to open \"app.log\": Logging to file disabled.");
    }
}

void Logger_closeLog() {
    // Close the log file
    fclose(loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE]);
}

void Logger_logV(const LogLevel logLevel, const char* format, const va_list args) {
    // Return if the log message is below the minimum log level
    if (logLevel < loggerState.logLevel) {
        return;
    }

    // Get the current timestamp
    char timestampString[14];
    Logger_getTimestampString(timestampString, sizeof(timestampString));

    // Print the message to each output file
    for (int i = 0; i < LOGGER_OUTPUT_STREAM_COUNT; i++) {
        // Skip this stream if it is null
        if (!loggerState.outputStreams[i]) {
            continue;
        }

        // Print the message
        fprintf(loggerState.outputStreams[i], "[%s (%s)]   ",
            Logger_getLogLevelString(logLevel),
            timestampString);
        vfprintf(loggerState.outputStreams[i], format, args);
        fprintf(loggerState.outputStreams[i], "\n");
    }
}

void Logger_log(const LogLevel logLevel, const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(logLevel, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_debug(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(LOGGER_LOG_LEVEL_DEBUG, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_info(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(LOGGER_LOG_LEVEL_INFO, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_warning(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(LOGGER_LOG_LEVEL_WARNING, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_error(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(LOGGER_LOG_LEVEL_ERROR, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_fatal(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    Logger_logV(LOGGER_LOG_LEVEL_FATAL, format, args);

    // End processing variable arguments
    va_end(args);
}

LogLevel Logger_getLogLevel() {
    return loggerState.logLevel;
}

void Logger_setLogLevel(const LogLevel logLevel) {
    loggerState.logLevel = logLevel;
}

void Logger_getTimestampString(char* buffer, const size_t bufferSize) {
    // Get elapsed milliseconds since application start
    const u64 elapsedMilliseconds = SDL_GetTicks();
    const u64 elapsedSeconds = elapsedMilliseconds / 1000;

    // Calculate time divisions
    const u64 milliseconds = elapsedMilliseconds % 1000;
    const u64 seconds = elapsedSeconds % 60;
    const u64 minutes = elapsedSeconds / 60 % 60;
    const u64 hours = elapsedSeconds / 3600;

    // Write the timestamp to the buffer
    snprintf(buffer, bufferSize,
        "%02llu:%02llu:%02llu:%03llu",
        hours, minutes, seconds, milliseconds);
}

const char* Logger_getLogLevelString(const LogLevel logLevel) {
    switch (logLevel) {
        case LOGGER_LOG_LEVEL_DEBUG:
            return "Debug";
        case LOGGER_LOG_LEVEL_INFO:
            return "Info ";
        case LOGGER_LOG_LEVEL_WARNING:
            return "Warn ";
        case LOGGER_LOG_LEVEL_ERROR:
            return "Error";
        case LOGGER_LOG_LEVEL_FATAL:
            return "Fatal";
        default:
            return "Unkwn";
    }
}
