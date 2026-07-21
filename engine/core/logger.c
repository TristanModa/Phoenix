#include "logger.h"

#include <stdio.h>
#include <mimalloc.h>
#include <time.h>
#include <SDL3/SDL.h>

#include "types.h"

static LoggerState loggerState;

static u64 getTimestampMS();
static void logFormatted(LogLevel logLevel, const char* format, va_list args);
static void mimallocOutputFunction(const char* msg, void* _);
static void sdlOutputFunction(void* _, int category, SDL_LogPriority priority, const char* msg);

void Logger_setProperties(const LoggerProperties *loggerProperties) {
    loggerState.logFilePath = loggerProperties->logFilePath;
}

void Logger_init(const LogLevel logLevel, const char* appName, const char* appVersion) {
    // Set the log level
    loggerState.logLevel = logLevel;

    // Set the logger outputs
    loggerState.outputStreams[LOGGER_OUTPUT_STREAM_STDOUT] = stdout;
    loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE] = fopen(loggerState.logFilePath, "w");

    // Get file open error if it failed
    const bool fileOpenFailed = !loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE];
    const int fileOpenErrorNum = errno;

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

    // Set the log init time
    loggerState.logInitTime = getTimestampMS();

    // Output log initialized
    Logger_info("Logger initialized.");
    Logger_pushIndent();
    if (fileOpenFailed) {
        Logger_error(
            "Failed to open log file \"%s\" for writing: %s.",
            loggerState.logFilePath, strerror(fileOpenErrorNum));
        Logger_info("Logging to file is disabled.");
    }
    Logger_popIndent();

    // Set library log output functions
    mi_register_output(mimallocOutputFunction, nullptr);
    SDL_SetLogOutputFunction(sdlOutputFunction, nullptr);
    SDL_LogPriority sdlLogLevel = loggerState.logLevel + 2;
    SDL_SetLogPriorities(sdlLogLevel);
}

void Logger_closeLog() {
    // Flush all output streams
    for (int i = 0; i < LOGGER_OUTPUT_STREAM_COUNT; i++) {
        fflush(loggerState.outputStreams[i]);
    }

    // Close the log file
    fclose(loggerState.outputStreams[LOGGER_OUTPUT_STREAM_LOG_FILE]);
}

void Logger_log(const LogLevel logLevel, const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(logLevel, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_debug(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOGGER_LOG_LEVEL_DEBUG, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_info(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOGGER_LOG_LEVEL_INFO, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_warning(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOGGER_LOG_LEVEL_WARNING, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_error(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOGGER_LOG_LEVEL_ERROR, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_fatal(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOGGER_LOG_LEVEL_FATAL, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_pushIndent() {
    loggerState.indentationLevel++;
}

void Logger_popIndent() {
    if (loggerState.indentationLevel == 0) {
        return;
    }
    loggerState.indentationLevel--;
}

LogLevel Logger_getLogLevel() {
    return loggerState.logLevel;
}

void Logger_setLogLevel(const LogLevel logLevel) {
    loggerState.logLevel = logLevel;
}

void Logger_getTimestampString(char* buffer, const size_t bufferSize) {
    // Get elapsed milliseconds since log init
    const u64 elapsedMilliseconds = getTimestampMS() - loggerState.logInitTime;
    const u64 elapsedSeconds = elapsedMilliseconds / 1000;

    // Calculate time divisions
    const u64 milliseconds = elapsedMilliseconds % 1000;
    const u64 seconds = elapsedSeconds % 60;
    const u64 minutes = elapsedSeconds / 60 % 60;
    const u64 hours = elapsedSeconds / 3600;

    // Write the timestamp to the buffer
    snprintf(buffer, bufferSize,
        "%02" PRIu64 ":%02" PRIu64 ":%02" PRIu64 ":%03" PRIu64,
        hours, minutes, seconds, milliseconds);
}

const char* Logger_getLogLevelString(const LogLevel logLevel) {
    switch (logLevel) {
        case LOGGER_LOG_LEVEL_DEBUG:
            return "Debug";
        case LOGGER_LOG_LEVEL_INFO:
            return "Info";
        case LOGGER_LOG_LEVEL_WARNING:
            return "Warning";
        case LOGGER_LOG_LEVEL_ERROR:
            return "Error";
        case LOGGER_LOG_LEVEL_FATAL:
            return "Fatal";
        default:
            return "Unknown";
    }
}

const char* Logger_getLogLevelLabel(LogLevel logLevel) {
    switch (logLevel) {
        case LOGGER_LOG_LEVEL_DEBUG:
            return "Debug";
        case LOGGER_LOG_LEVEL_INFO:
            return "Info";
        case LOGGER_LOG_LEVEL_WARNING:
            return "Warn";
        case LOGGER_LOG_LEVEL_ERROR:
            return "Error";
        case LOGGER_LOG_LEVEL_FATAL:
            return "Fatal";
        default:
            return "Unkwn";
    }
}

u64 getTimestampMS() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME_COARSE, &spec);
    return (u64)spec.tv_sec * 1000 + spec.tv_nsec / 1000000;
}

void logFormatted(const LogLevel logLevel, const char* format, va_list args) {
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

        // Add relevant ANSI color code if the stream is stdout
        if (i == LOGGER_OUTPUT_STREAM_STDOUT) {
            const char* escapeCode;
            switch (logLevel) {
                case LOGGER_LOG_LEVEL_DEBUG:
                    escapeCode = ANSI_DEBUG;
                    break;
                case LOGGER_LOG_LEVEL_INFO:
                    escapeCode = ANSI_INFO;
                    break;
                case LOGGER_LOG_LEVEL_WARNING:
                    escapeCode = ANSI_WARNING;
                    break;
                case LOGGER_LOG_LEVEL_ERROR:
                    escapeCode = ANSI_ERROR;
                    break;
                case LOGGER_LOG_LEVEL_FATAL:
                    escapeCode = ANSI_FATAL;
                    break;
                default:
                    escapeCode = "";
                    break;
            }
            fprintf(loggerState.outputStreams[i], "%s", escapeCode);
        }

        // Print the message label
        fprintf(loggerState.outputStreams[i], "[%-5s (%s)]",
            Logger_getLogLevelLabel(logLevel),
            timestampString);

        // Print indentation
        for (int j = 0; j < loggerState.indentationLevel + 1; j++) {
            fprintf(loggerState.outputStreams[i], "    ");
        }

        // Print the message
        vfprintf(loggerState.outputStreams[i], format, args);
        fprintf(loggerState.outputStreams[i], "\n");

        // Print the ANSI reset code if the stream is stdout
        if (i == LOGGER_OUTPUT_STREAM_STDOUT) {
            fprintf(loggerState.outputStreams[i], ANSI_RESET);
        }
    }
}

void mimallocOutputFunction(const char* msg, void* _) {
    // Return if the message is null or empty
    if (!msg || msg[0] == '\0') {
        return;
    }

    // Create the buffer
    constexpr size_t BUFFER_SIZE = 128;
    static char buffer[BUFFER_SIZE] = {};
    static size_t bufferPos = 0;

    // Write each character from the msg to the buffer
    size_t i = 0;
    size_t msgLen = strlen(msg);
    bool flushBuffer = false;
    while (i < msgLen) {
        char c = msg[i];

        // Flush the buffer if filled
        if (bufferPos >= BUFFER_SIZE - 1) {
            buffer[BUFFER_SIZE - 1] = '\0';
            flushBuffer = true;
            break;
        }

        // Flush the buffer on newline
        if (c == '\n') {
            buffer[bufferPos] = '\0';
            flushBuffer = true;
            break;
        }

        // Write the character to the buffer
        buffer[bufferPos] = c;

        // Increment iterators
        i++;
        bufferPos++;
    }

    // Return if the buffer should not be flushed yet
    if (!flushBuffer) {
        return;
    }

    // Determine the log level of the message
    LogLevel logLevel;
    if (strstr(buffer, "warning") != nullptr) {
        logLevel = LOGGER_LOG_LEVEL_WARNING;
    } else if (strstr(buffer, "error") != nullptr) {
        logLevel = LOGGER_LOG_LEVEL_ERROR;
    } else {
        logLevel = LOGGER_LOG_LEVEL_INFO;
    }

    // Output the message and clear the buffer
    if (buffer[0] != '\n') {
        Logger_log(logLevel, "%s", buffer);
    }
    memset(buffer, '\0', BUFFER_SIZE);
    bufferPos = 0;
}

void sdlOutputFunction(void* _, int category, SDL_LogPriority priority, const char* msg) {
    // Return if the message is null
    if (!msg) {
        return;
    }

    // Get the category string
    const char* categoryStr;
    switch ((SDL_LogCategory)category) {
        case SDL_LOG_CATEGORY_APPLICATION:
            categoryStr = "Application";
            break;
        case SDL_LOG_CATEGORY_ERROR:
            categoryStr = "Error";
            break;
        case SDL_LOG_CATEGORY_ASSERT:
            categoryStr = "Assert";
            break;
        case SDL_LOG_CATEGORY_SYSTEM:
            categoryStr = "System";
            break;
        case SDL_LOG_CATEGORY_AUDIO:
            categoryStr = "Audio";
            break;
        case SDL_LOG_CATEGORY_VIDEO:
            categoryStr = "Video";
            break;
        case SDL_LOG_CATEGORY_RENDER:
            categoryStr = "Render";
            break;
        case SDL_LOG_CATEGORY_INPUT:
            categoryStr = "Input";
            break;
        case SDL_LOG_CATEGORY_TEST:
            categoryStr = "Test";
            break;
        case SDL_LOG_CATEGORY_GPU:
            categoryStr = "GPU";
            break;
        case SDL_LOG_CATEGORY_CUSTOM:
            categoryStr = "Custom";
            break;
        default:
            categoryStr = "Unknown";
            break;
    }

    // Get the log level string
    LogLevel logLevel = priority - 2;
    const char* logLevelString = Logger_getLogLevelString(logLevel);

    // Output the message
    Logger_log(logLevel, "SDL %s: %s: %s", categoryStr, logLevelString, msg);
}