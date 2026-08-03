#include "logger.h"

#include <assert.h>
#include <dcimgui.h>
#include <stdio.h>
#include <mimalloc.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

typedef void (*LogSinkFn)(LogLevel, const char*);

static struct {
    LogLevel logLevel;
    LogSinkFn logSinks[LOG_SINK_COUNT];

    FILE* logFile;
    struct {
        size_t head;
        size_t tail;
        char buffer[LOG_HISTORY_BUFFER_SIZE];
        size_t traversePosition;
    } history;

    u64 logInitTime;
    u8 indentationLevel;
} loggerState;

static u64 getTimestampMS();
static void logFormatted(LogLevel logLevel, const char* format, va_list args);

static void writeStdout(LogLevel logLevel, const char* msg);
static void writeLogFile(LogLevel _, const char* msg);
static void writeLogHistoryBuffer(LogLevel _, const char* msg);

static void mimallocOutputFunction(const char* msg, void* _);
static void sdlOutputFunction(void* _, int category, SDL_LogPriority priority, const char* msg);

void Logger_create(const LogLevel logLevel, const char* appName, const char* appVersion) {
    // Initialize logger state
    loggerState = (typeof(loggerState)){
        .logLevel = logLevel,
        .logSinks = { writeStdout, writeLogFile, writeLogHistoryBuffer },
        .logFile = fopen(LOG_PATH, "w"),
        .history = {}
    };

    // Write the log header to each log sink
    char logHeader[MAX_LOG_MESSAGE_LENGTH];
    snprintf(logHeader, sizeof(logHeader), "%s Log %s", appName, appVersion);
    for (u8 i = 0; i < LOG_SINK_COUNT; i++) {
        loggerState.logSinks[i](LOG_LEVEL_UNKNOWN, logHeader);
    }

    // Set library log output functions
    mi_register_output(mimallocOutputFunction, nullptr);
    SDL_SetLogOutputFunction(sdlOutputFunction, nullptr);
    const SDL_LogPriority sdlLogLevel = loggerState.logLevel + 2;
    SDL_SetLogPriorities(sdlLogLevel);

    // Set the log init time
    loggerState.logInitTime = getTimestampMS();

    // Log initialization completion
    Logger_info("Logger initialized");
    if (!loggerState.logFile) {
        Logger_pushIndent();
        Logger_error("Failed to open %s for writing. Logging to file disabled", LOG_PATH);
        Logger_popIndent();
    }
}

void Logger_destroy() {
    // Close the log file
    if (loggerState.logFile) fclose(loggerState.logFile);
}

void Logger_addLogSink(LogSinkID sinkID, LogSinkFn sinkFn) {
    // Return if the sink ID is invalid
    if (sinkID >= LOG_SINK_COUNT) {
        Logger_error("Failed to add log sink: Invalid sink ID");
        return;
    }

    // Set the sink function
    loggerState.logSinks[sinkID] = sinkFn;
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
    logFormatted(LOG_LEVEL_DEBUG, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_info(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOG_LEVEL_INFO, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_warning(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOG_LEVEL_WARNING, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_error(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOG_LEVEL_ERROR, format, args);

    // End processing variable arguments
    va_end(args);
}

void Logger_fatal(const char* format, ...) {
    // Begin processing variable arguments
    va_list args = {};
    va_start(args, format);

    // Pass the log message to the general log function
    logFormatted(LOG_LEVEL_FATAL, format, args);

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

const char* Logger_getLogLevelString(const LogLevel logLevel) {
    switch (logLevel) {
        case LOG_LEVEL_DEBUG:
            return "Debug";
        case LOG_LEVEL_INFO:
            return "Info";
        case LOG_LEVEL_WARNING:
            return "Warning";
        case LOG_LEVEL_ERROR:
            return "Error";
        case LOG_LEVEL_FATAL:
            return "Fatal";
        default:
            return "Unknown";
    }
}

void Logger_beginTraverseHistoryBuffer() {
    loggerState.history.traversePosition = loggerState.history.head;
}

const char* Logger_getNextHistoryLine(size_t* n) {
    while (loggerState.history.traversePosition < loggerState.history.tail) {
        const size_t index = loggerState.history.traversePosition & LOG_HISTORY_BUFFER_SIZE - 1;
        const char* begin = loggerState.history.buffer + index;

        // Skip wrap padding
        if (*begin == '\0') {
            loggerState.history.traversePosition += LOG_HISTORY_BUFFER_SIZE - index;
            continue;
        }

        const size_t len = strlen(begin);
        loggerState.history.traversePosition += len + 1;

        if (n) *n = len;
        return begin;
    }

    if (n) *n = 0;
    return nullptr;
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

    // Get elapsed milliseconds since log init
    const u64 elapsedMilliseconds = getTimestampMS() - loggerState.logInitTime;
    const u64 elapsedSeconds = elapsedMilliseconds / 1000;

    // Get the log level label
    const char* logLevelLabel;
    switch (logLevel) {
        case LOG_LEVEL_DEBUG:
            logLevelLabel = "Debug";
            break;
        case LOG_LEVEL_INFO:
            logLevelLabel = "Info";
            break;
        case LOG_LEVEL_WARNING:
            logLevelLabel = "Warn";
            break;
        case LOG_LEVEL_ERROR:
            logLevelLabel = "Error";
            break;
        case LOG_LEVEL_FATAL:
            logLevelLabel = "Fatal";
            break;
        default:
            logLevelLabel = "Unkwn";
            break;
    }

    // Calculate time divisions
    const u64 milliseconds = elapsedMilliseconds % 1000;
    const u64 seconds = elapsedSeconds % 60;
    const u64 minutes = elapsedSeconds / 60 % 60;
    const u64 hours = elapsedSeconds / 3600;

    // Create the message buffer
    size_t bufferPos = 0;
    char messageBuffer[MAX_LOG_MESSAGE_LENGTH];

    // Write the message label
    int written = snprintf(
        messageBuffer,
        sizeof(messageBuffer),
        "[%-5s (%02" PRIu64 ":%02" PRIu64 ":%02" PRIu64 ".%03" PRIu64 ")]",
        logLevelLabel,
        hours, minutes, seconds, milliseconds);

    // Add the written characters to the buffer position
    if (written > 0) bufferPos += SDL_min((size_t)written, MAX_LOG_MESSAGE_LENGTH - bufferPos);

    // Write indentation
    const size_t numSpaces = SDL_min((loggerState.indentationLevel + 1) * 4, MAX_LOG_MESSAGE_LENGTH - bufferPos);
    memset(messageBuffer + bufferPos, ' ', numSpaces);
    bufferPos += numSpaces;

    // Write the formatted message
    written = vsnprintf(
        messageBuffer + bufferPos,
        sizeof(messageBuffer) - bufferPos,
        format, args);

    // Write message to all log sinks
    for (u8 i = 0; i < LOG_SINK_COUNT; i++) {
        // Skip null log sink functions
        if (!loggerState.logSinks[i]) {
            continue;
        }

        // Call the log sink function
        loggerState.logSinks[i](logLevel, messageBuffer);
    }
}

void writeStdout(LogLevel logLevel, const char* msg) {
    constexpr char CODE_RESET[] = "\x1B[39m";
    constexpr char CODE_DEBUG[] = "\x1B[38;5;14m";
    constexpr char CODE_INFO[] = "\x1B[38;5;15m";
    constexpr char CODE_WARNING[] = "\x1B[38;5;11m";
    constexpr char CODE_ERROR[] = "\x1B[38;5;1m";
    constexpr char CODE_FATAL[] = "\x1B[38;5;88m";

    // Write the color escape code corresponding to the log level
    const char* escapeCode;
    switch (logLevel) {
        case LOG_LEVEL_DEBUG:
            escapeCode = CODE_DEBUG;
            break;
        case LOG_LEVEL_INFO:
            escapeCode = CODE_INFO;
            break;
        case LOG_LEVEL_WARNING:
            escapeCode = CODE_WARNING;
            break;
        case LOG_LEVEL_ERROR:
            escapeCode = CODE_ERROR;
            break;
        case LOG_LEVEL_FATAL:
            escapeCode = CODE_FATAL;
            break;
        default:
            escapeCode = CODE_RESET;
            break;
    }
    fputs(escapeCode, stdout);

    // Write the message
    fputs(msg, stdout);

    // Write the reset escape code
    fputs(CODE_RESET, stdout);
    fputc('\n', stdout);
}

void writeLogFile(LogLevel _, const char* msg) {
    // Return if the log file is null
    if (!loggerState.logFile) {
        return;
    }

    // Write the buffer
    fputs(msg, loggerState.logFile);
    fputc('\n', loggerState.logFile);
}

void writeLogHistoryBuffer(LogLevel _, const char* msg) {
    // Get the message length with a terminator
    const size_t msgLen = strlen(msg) + 1;
    assert(msgLen <= LOG_HISTORY_BUFFER_SIZE);

    // Advance the tail to the start of the buffer if the message would not fit at the end
    const size_t tailIndex = loggerState.history.tail & LOG_HISTORY_BUFFER_SIZE - 1;
    if (tailIndex + msgLen > LOG_HISTORY_BUFFER_SIZE) {
        memset(loggerState.history.buffer + tailIndex, 0, LOG_HISTORY_BUFFER_SIZE - tailIndex);
        loggerState.history.tail += LOG_HISTORY_BUFFER_SIZE - tailIndex;
    }

    // Advance the head by messages until it is no longer overlapping with the tail
    while (loggerState.history.tail - loggerState.history.head + msgLen > LOG_HISTORY_BUFFER_SIZE) {
        const size_t headIndex = loggerState.history.head & LOG_HISTORY_BUFFER_SIZE - 1;
        if (loggerState.history.buffer[headIndex] == '\0') {
            loggerState.history.head += LOG_HISTORY_BUFFER_SIZE - headIndex;
            continue;
        }
        loggerState.history.head += strlen(loggerState.history.buffer + headIndex) + 1;
    }

    // Write the message to the buffer
    const size_t writePos = loggerState.history.tail & LOG_HISTORY_BUFFER_SIZE - 1;
    memcpy(loggerState.history.buffer + writePos, msg, msgLen);

    // Move the tail forward by the length of the message
    loggerState.history.tail += msgLen;
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
    const size_t msgLen = strlen(msg);
    bool flushBuffer = false;
    while (i < msgLen) {
        const char c = msg[i];

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
        logLevel = LOG_LEVEL_WARNING;
    } else if (strstr(buffer, "error") != nullptr) {
        logLevel = LOG_LEVEL_ERROR;
    } else {
        logLevel = LOG_LEVEL_INFO;
    }

    // Output the message and clear the buffer
    if (strlen(buffer) != 0) {
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
    const LogLevel logLevel = priority - 2;
    const char* logLevelStr = Logger_getLogLevelString(logLevel);

    // Output the message
    Logger_log(logLevel, "SDL %s: %s: %s", categoryStr, logLevelStr, msg);
}