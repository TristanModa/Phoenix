#ifndef ENGINE_CORE_TYPES_H
#define ENGINE_CORE_TYPES_H

#include <stdint.h>

/**
 * A signed 8-bit integer
 */
typedef int8_t i8;
/**
 * A signed 16-bit integer
 */
typedef int16_t i16;
/**
 * A signed 32-bit integer
 */
typedef int32_t i32;
/**
 * A signed 64-bit integer
 */
typedef int64_t i64;

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
 * A function pointer for a void function with no arguments
 */
typedef void (*VoidFn)();

#endif //ENGINE_CORE_TYPES_H
