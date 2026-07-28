#ifndef ENGINE_VTIME_H
#define ENGINE_VTIME_H

#include "types.h"

/**
 * The target amount of ticks to process in a second
 */
constexpr float TICKS_PER_SECOND = 240;

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

#endif //ENGINE_VTIME_H
