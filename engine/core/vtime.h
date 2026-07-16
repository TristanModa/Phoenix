#ifndef ENGINE_CORE_VTIME_H
#define ENGINE_CORE_VTIME_H

#include "types.h"

/**
 * Defines a configuration for the time subsystem
 */
typedef struct timeProperties {
    float targetTicksPerSecond;
    float maxTickProcessingTimePerFrame;
} TimeProperties;

/**
 * The state of the time subsystem
 */
typedef struct timeState {
    /**
     * The time elapsed since the time system was initialized in nanoseconds
     */
    u64 nowNS;
    /**
     * The time elapsed since the time system was initialized as of last update in nanoseconds
     */
    u64 prevNS;

    /**
     * The time elapsed since the time system was initialized in seconds
     */
    float currentTime;
    /**
     * The time elapsed
     */
    float deltaTime;
    /**
     * The amount of frames processed per second
     */
    float framesPerSecond;


    /**
     * The rate at which ticks should be processed in seconds
     */
    float tickRate;
    /**
     * The maximum amount of time per frame ticks should be allowed to be processed for
     */
    float maxTickProcessingTimePerFrame;

    /**
     * The amount of ticks that have been processed since the time system was initialized
     */
    u32 currentTick;
    /**
     * The amount of time that has elapsed since the previous tick in seconds
     */
    float tickTimer;
    /**
     * The amount of ticks processed last frame
     */
    u32 ticksProcessedThisFrame;
} TimeState;

/**
 * Sets the properties of the time subsystem.
 * Should be called after Application_create but prior to Application_run.
 * @param timeProperties The properties struct to use
 */
void Time_setProperties(const TimeProperties* timeProperties);

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
 * Gets the framerate of the application
 * @return The framerate in frames per second
 */
float Time_getFPS();
/**
 * Gets the elapsed ticks since the application was initialized
 * @return The elapsed ticks
 */
u32 Time_getCurrentTick();

#endif //ENGINE_CORE_VTIME_H
