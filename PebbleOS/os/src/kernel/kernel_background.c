/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_background.c
 * @author Evan Stoddard
 * @brief Kernel background thread. Runs at a lower priority than the kernel
 * main thread and is intended to handle deferred background work. Currently
 * sleeps indefinitely as a placeholder.
 */

#include "kernel_background.h"
#include "pebble_thread.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kernel_bg);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private module instance
 */
static struct {
  PebbleThread_t *thread;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Kernel background thread entry point. Sleeps indefinitely, waking
 * only when explicitly signalled by other kernel code to perform deferred work.
 *
 * @param args Unused thread argument
 */
void prv_thread_entry(void *args) {
  LOG_INF("Kernel background thread starting...");

  while (true) {
    k_sleep(K_FOREVER);
  }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kernel_background_init(void) {
  prv_inst.thread = pebble_thread_create(PebbleThread_KernelBackground, "Kernel BG",
                                         CONFIG_KERNEL_BACKGROUND_STACK_SIZE,
                                         CONFIG_KERNEL_BACKGROUND_PRIORITY, prv_thread_entry, NULL);

  __ASSERT(prv_inst.thread, "Unable to allocate kernel background thread.");

  return 0;
}

PebbleThread_t *kernel_background_thread(void) {
  return prv_inst.thread;
}
