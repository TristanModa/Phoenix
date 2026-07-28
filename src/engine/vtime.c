#include "vtime.h"

#include <SDL3/SDL.h>

#include "logger.h"

constexpr float TICK_RATE = 1 / TICKS_PER_SECOND;

static struct {
    u64 nowNS;
    u64 prevNS;

    float currentTime;
    float deltaTime;

    u32 currentTick;
    float tickTimer;
} timeState;

void Time_init() {
    timeState = (typeof(timeState)) {
        .nowNS = 0,
        .prevNS = 0,
        .currentTime = 0,
        .deltaTime = 0,
        .currentTick = 0,
        .tickTimer = 0
    };
}

void Time_update() {
    // Update the internal timer
    timeState.prevNS = timeState.nowNS;
    timeState.nowNS = SDL_GetTicksNS();

    // Update the current time, delta time, and frames per second
    timeState.currentTime = (float)timeState.nowNS * 1e-9f;
    timeState.deltaTime = (float)(timeState.nowNS - timeState.prevNS) * 1e-9f;

    // Add delta time to the tick timer
    timeState.tickTimer += timeState.deltaTime;
}

bool Time_consumeTick() {
    // Return false if there is not enough time to complete a tick
    if (timeState.tickTimer < TICK_RATE) {
        return false;
    }

    // Consume one tick's worth of time
    timeState.tickTimer -= TICK_RATE;
    timeState.currentTick++;
    return true;
}

float Time_getCurrentTime() {
    return timeState.currentTime;
}

float Time_getDeltaTime() {
    return timeState.deltaTime;
}

u32 Time_getCurrentTick() {
    return timeState.currentTick;
}