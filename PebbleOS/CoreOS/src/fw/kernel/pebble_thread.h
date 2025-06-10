/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_thread.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef pebble_thread_h
#define pebble_thread_h

#include <zephyr/kernel.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum
{
    PEBBLE_THREAD_TYPE_KERNEL_MAIN,
    PEBBLE_THREAD_TYPE_KERNEL_BACKGROUND,
    PEBBLE_THREAD_TYPE_APP,
    PEBBLE_THREAD_TYPE_BACKGROUND,
    PEBBLE_THREAD_TYPE_TIMER,
} PebbleThreadType;

typedef void (*PebbleThreadEntryPoint)(void *args);

/**
 * @class PebbleThread
 * @brief Conscious decision to make this an opaque type and have all threads allocated due to nuances in spinning up
 * threads and stacks. Can easily be swayed to have a static and allocated API, but will need to define helper macros
 * for allocating/aligning stack space.
 *
 */
struct PebbleThread;

/**
 * @typedef PebbleThread
 * @brief [TODO:description]
 *
 */
typedef struct PebbleThread *PebbleThread;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param type [TODO:parameter]
 * @param stack_size [TODO:parameter]
 * @param priority [TODO:parameter]
 * @param entry_point [TODO:parameter]
 * @return [TODO:return]
 */
PebbleThread pebble_thread_create_thread(PebbleThreadType type, size_t stack_size, int32_t priority,
                                         PebbleThreadEntryPoint entry_point);

/**
 * @brief [TODO:description]
 *
 * @param thread [TODO:parameter]
 */
void pebble_thread_destroy_thread(PebbleThread thread);

#ifdef __cplusplus
}
#endif
#endif /* pebble_thread_h */
