#ifndef ENGINE_CORE_LOGGER_H
#define ENGINE_CORE_LOGGER_H

#include <stdio.h>

typedef enum logOutputStream {
    LOGGER_OUTPUT_STREAM_STDOUT,
    LOGGER_OUTPUT_STREAM_LOG_FILE,
    LOGGER_OUTPUT_STREAM_COUNT
} LogOutputStream;

typedef enum logLevel {
    LOGGER_LOG_LEVEL_DEBUG,
    LOGGER_LOG_LEVEL_INFO,
    LOGGER_LOG_LEVEL_WARNING,
    LOGGER_LOG_LEVEL_ERROR,
    LOGGER_LOG_LEVEL_FATAL
} LogLevel;

typedef struct loggerState {
    LogLevel logLevel;
    FILE* outputStreams[LOGGER_OUTPUT_STREAM_COUNT];
} LoggerState;

void Logger_init(LogLevel logLevel, const char* appName, const char* appVersion);
void Logger_closeLog();

void Logger_logV(LogLevel logLevel, const char* format, va_list args);
void Logger_log(LogLevel logLevel, const char* format, ...)  __attribute__((format(printf, 2, 3)));

void Logger_debug(const char* format, ...) __attribute__((format(printf, 1, 2)));
void Logger_info(const char* format, ...) __attribute__((format(printf, 1, 2)));
void Logger_warning(const char* format, ...) __attribute__((format(printf, 1, 2)));
void Logger_error(const char* format, ...) __attribute__((format(printf, 1, 2)));
void Logger_fatal(const char* format, ...) __attribute__((format(printf, 1, 2)));

LogLevel Logger_getLogLevel();
void Logger_setLogLevel(LogLevel logLevel);

void Logger_getTimestampString(char* buffer, size_t bufferSize);
const char* Logger_getLogLevelString(LogLevel logLevel);

#endif //ENGINE_CORE_LOGGER_H
