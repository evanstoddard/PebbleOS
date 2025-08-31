/*
 * Copyright (C) Ovyl
 */

/**
 * @file system_thread.c
 * @author Evan Stoddard
 * @brief
 */

#include "system_thread.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "pebble_thread.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(system_thread);

#define SYSTEM_THREAD_STACK_SIZE_BYTES (1024U)

#define SYSTEM_THREAD_PRIORITY 4

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct
{
    PebbleThread thread;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief System thread entry point
 *
 * @param args Unused
 */
static void prv_thread_entry(void *args)
{
    (void)args;

    LOG_INF("Kernel background thread started.");

    while (true)
    {
        k_sleep(K_FOREVER);
    }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void system_thread_init(void)
{
    prv_inst.thread = pebble_thread_create_thread(PEBBLE_THREAD_TYPE_KERNEL_BACKGROUND, SYSTEM_THREAD_STACK_SIZE_BYTES,
                                                  SYSTEM_THREAD_PRIORITY, prv_thread_entry, NULL);
}
