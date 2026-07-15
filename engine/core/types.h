#ifndef ENGINE_CORE_TYPES_H
#define ENGINE_CORE_TYPES_H

#include <stdint.h>

/**
 * An unsigned 8-bit integer
 */
typedef uint8_t u8;
/**
 * An unsigned 16-bit integer
 */
typedef uint16_t u16;
/**
 * An unsigned 32-bit integer
 */
typedef uint32_t u32;
/**
 * An unsigned 64-bit integer
 */
typedef uint64_t u64;

/**
 * A signed 8-bit integer
 */
typedef int8_t s8;
/**
 * A signed 16-bit integer
 */
typedef int16_t s16;
/**
 * A signed 32-bit integer
 */
typedef int32_t s32;
/**
 * A signed 64-bit integer
 */
typedef int64_t s64;

/**
 * A function pointer for a void function with no arguments
 */
typedef void (*VoidFunc)(void);

#endif //ENGINE_CORE_TYPES_H
