#ifndef ENGINE_CORE_VTIME_H
#define ENGINE_CORE_VTIME_H

#include "types.h"

typedef struct timeProperties {
    float targetTicksPerSecond;
} TimeProperties;

/**
 * The state of the time subsystem
 */
typedef struct timeState {
    u64 nowNS;
    u64 prevNS;

    float currentTime;
    float deltaTime;
    float framesPerSecond;

    float tickRate;
    float tickTimer;
    u32 currentTick;
} TimeState;

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
 * Consumes one tick from the tick timer
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


#endif //ENGINE_CORE_VTIME_H
