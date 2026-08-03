#include "vtime.h"

#include <math.h>
#include <SDL3/SDL.h>

constexpr float TICK_RATE = 1 / TICKS_PER_SECOND;

static struct {
    u64 nowNS;
    u64 prevNS;

    float currentTime;
    float deltaTime;

    u32 currentTick;
    float tickTimer;

    Profiler profilers[PROFILER_COUNT];
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

    // Update profilers
    for (ProfilerID i = 0; i < PROFILER_COUNT; i++) {
        Profiler* profiler = &timeState.profilers[i];
        const u64 timeNS = profiler->endTimeNS - profiler->startTimeNS;

        // Avoid calculating delta profiling on the first frame
        if (profiler->startTimeNS == 0) {
            continue;
        }

        // Calculate the current, average, and peak
        profiler->current = (float)timeNS * 1e-6f;
        profiler->average += (profiler->current - profiler->average) * PROFILER_AVERAGE_WEIGHT;
        if (profiler->current > profiler->decayedPeak) {
            profiler->peak = profiler->current;
            profiler->decayedPeak = profiler->current;
        }

        // Update the decayed peak
        const float peakDecay = expf(-timeState.deltaTime / PROFILER_PEAK_DECAY_HALF_LIFE);
        profiler->decayedPeak = profiler->current + (profiler->decayedPeak - profiler->current) * peakDecay;
    }
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

void Time_startProfiler(ProfilerID profilerID) {
    timeState.profilers[profilerID].startTimeNS = SDL_GetTicksNS();
}

void Time_endProfiler(ProfilerID profilerID) {
    timeState.profilers[profilerID].endTimeNS = SDL_GetTicksNS();
}

const Profiler* Time_getProfiler(ProfilerID profilerID) {
    return &timeState.profilers[profilerID];
}