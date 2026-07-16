#include "vtime.h"

#include <math.h>
#include <time.h>
#include <SDL3/SDL.h>

#include "logger.h"

static TimeState timeState;

void Time_setProperties(const TimeProperties* timeProperties) {
    // Calculate the tick rate
    timeState.tickRate = 1 / timeProperties->targetTicksPerSecond;
}

void Time_init() {
    Logger_info("Initializing time subsystem...");
    Logger_pushIndent();

    // Initialize time state
    timeState.nowNS = 0;
    timeState.prevNS = 0;
    timeState.currentTime = 0;
    timeState.deltaTime = 0;
    timeState.framesPerSecond = 0;
    timeState.tickTimer = 0;
    timeState.currentTick = 0;

    // Throw an error on invalid tick rate
    if (timeState.tickRate <= 0 || isinf(timeState.tickRate) || isnan(timeState.tickRate)) {
        Logger_fatal("Failed to initialize time: Tick rate of %.2g is invalid.", timeState.tickRate);
        exit(-1);
    }

    // Pop the log indent
    Logger_popIndent();
}

void Time_update() {
    // Update the internal timer
    timeState.prevNS = timeState.nowNS;
    timeState.nowNS = SDL_GetTicksNS();

    // Update the current time, delta time, and frames per second
    timeState.currentTime = (float)timeState.nowNS * 1e-9f;
    timeState.deltaTime = (float)(timeState.nowNS - timeState.prevNS) * 1e-9f;
    timeState.framesPerSecond = 1.0f / timeState.deltaTime;

    // Add delta time to the tick timer
    timeState.tickTimer += timeState.deltaTime;
}

float Time_getCurrentTime() {
    return timeState.currentTime;
}

float Time_getDeltaTime() {
    return timeState.deltaTime;
}

float Time_getFPS() {
    return timeState.framesPerSecond;
}

bool Time_consumeTick() {
    // Return false if there is not enough time to complete a tick
    if (timeState.tickTimer < timeState.tickRate) {
        return false;
    }

    // Consume one tick's worth of time
    timeState.tickTimer -= timeState.tickRate;
    return true;
}
