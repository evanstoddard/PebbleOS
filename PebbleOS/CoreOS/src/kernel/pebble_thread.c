/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_thread.c
 * @author Evan Stoddard
 * @brief
 */

#include "pebble_thread.h"

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pebble_thread);

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @typedef PebbleThread
 * @brief [TODO:description]
 *
 */
struct PebbleThread
{
    struct k_thread thread;
    k_tid_t thread_id;
    size_t stack_size_bytes;
    void *stack;
    int32_t priority;
    bool privileged;
    PebbleThreadEntryPoint entry_point;
    PebbleThreadType type;
};

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Zephyr threads accept three args.  Wrapping to only accept one arg.
 *
 * @param entry_point Entry point to actual thread
 * @param thread_args Arguments for thread
 * @param unused Unused
 */
void prv_thread_entry_point(void *entry_point, void *thread_args, void *unused)
{
    (void)unused;

    PebbleThreadEntryPoint _entry_point = (PebbleThreadEntryPoint)entry_point;

    _entry_point(thread_args);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

PebbleThread pebble_thread_create_thread(PebbleThreadType type, size_t stack_size, int32_t priority,
                                         PebbleThreadEntryPoint entry_point)
{
    struct PebbleThread *thread = NULL;

    thread = k_malloc(sizeof(struct PebbleThread));
    memset(thread, 0, sizeof(struct PebbleThread));

    thread->stack_size_bytes = stack_size;
    thread->priority = priority;
    thread->type = type;
    thread->entry_point = entry_point;

    if (thread == NULL)
    {
        LOG_ERR("Unable to allocate PebbleThread.");
        return NULL;
    }

    thread->stack = k_malloc(K_THREAD_STACK_LEN(stack_size));

    if (thread->stack == NULL)
    {
        LOG_ERR("Unable to allocate thread stack.");
        k_free(thread);
        return NULL;
    }

    thread->thread_id =
        k_thread_create(&thread->thread, thread->stack, K_THREAD_STACK_LEN(thread->stack_size_bytes),
                        prv_thread_entry_point, thread->entry_point, NULL, NULL, thread->priority, 0, K_NO_WAIT);

    thread->thread.custom_data = thread;

    return thread;
}

void pebble_thread_destroy_thread(PebbleThread thread)
{
}

PebbleThread pebble_thread_current_thread(void)
{
    struct k_thread *current_thread = k_current_get();

    // Zephyr also spins up threads for services like timers and drivers.  Must be able to handle these cases...
    // These threads might also use the custom_data property so it is theoretically possible that custom_data might
    // not be NULL and not point to a PebbleThread instance... might consider adding a magic value to the top of this?
    // Valid but potentially expensive alternative is to maintain a linked list of instantiate threads and iterate
    // through this list finding an instance with a matching thread ID.
    if (current_thread->custom_data == NULL)
    {
        return NULL;
    }

    PebbleThread handle = (PebbleThread)current_thread->custom_data;

    return handle;
}

PebbleThreadType pebble_thread_current_thread_type(void)
{
    struct PebbleThread *_thread = pebble_thread_current_thread();

    return _thread->type;
}
