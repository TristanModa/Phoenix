#include "vtime.h"

#include <SDL3/SDL.h>

static TimeState timeState;

void Time_init() {
    timeState.nowNS = 0;
    timeState.prevNS = 0;

    timeState.currentTime = 0;
    timeState.deltaTime = 0;
    timeState.framesPerSecond = 0;

    timeState.tickRate = 1 / 240.0f;
    timeState.tickTimer = 0;
    timeState.currentTick = 0;
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
