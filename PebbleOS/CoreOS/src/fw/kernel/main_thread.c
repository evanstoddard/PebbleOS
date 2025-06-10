/*
 * Copyright (C) Ovyl
 */

/**
 * @file main_thread.c
 * @author Evan Stoddard
 * @brief
 */

#include "main_thread.h"

#include <zephyr/logging/log.h>

#include "pebble_thread.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define MAIN_THREAD_STACK_SIZE_BYTES 1024U

#define MAIN_THREAD_PRIORITY 4

LOG_MODULE_REGISTER(main_thread);

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct
{
    PebbleThread main_thread;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Initial setup of kernel
 */
void prv_setup(void)
{
}

/**
 * @brief Main thread entry point
 *
 * @param args Unused
 */
void prv_thread_entry(void *args)
{
    (void)args;

    prv_setup();
    LOG_INF("Main kernel thread initialized.");
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

void main_thread_init(void)
{
    prv_inst.main_thread = pebble_thread_create_thread(PEBBLE_THREAD_TYPE_KERNEL_MAIN, MAIN_THREAD_STACK_SIZE_BYTES,
                                                       MAIN_THREAD_PRIORITY, prv_thread_entry);
}
