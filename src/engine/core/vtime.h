#ifndef ENGINE_CORE_VTIME_H
#define ENGINE_CORE_VTIME_H

#include "types.h"

/**
 * The target amount of ticks to process in a second
 */
constexpr float TICKS_PER_SECOND = 240;

constexpr float PROFILER_AVERAGE_WEIGHT = 0.05f;

constexpr float PROFILER_PEAK_DECAY_HALF_LIFE = 5.0f;

typedef enum profilerID : u8 {
	PROFILER_FRAME_DELTA,
	PROFILER_UPDATE,
	PROFILER_TICK,
	PROFILER_RENDER,
	PROFILER_COUNT
} ProfilerID;

typedef struct profiler {
	u64 startTimeNS;
	u64 endTimeNS;

	float current;
	float average;
	float peak;

	float decayedPeak;
} Profiler;

/**
 * Initializes the time subsystem
 */
void Time_init();

/**
 * Updates the time subsystem
 */
void Time_update();

/**
 * Consumes one tick from the tick timer if a ticks worth or more time remains.
 * @return True if a tick was consumed
 */
bool Time_consumeTick();

/**
 * Gets the elapsed time since the application was initialized
 * @return The elapsed time in seconds
 */
float Time_getCurrentTime();
/**
 * Gets the elapsed time since the previous frame
 * @return The elapsed time in seconds
 */
float Time_getDeltaTime();
/**
 * Gets the elapsed ticks since the application was initialized
 * @return The elapsed ticks
 */
u32 Time_getCurrentTick();

void Time_startProfiler(ProfilerID profilerID);
void Time_endProfiler(ProfilerID profilerID);
const Profiler* Time_getProfiler(ProfilerID profilerID);

#endif //ENGINE_CORE_VTIME_H
