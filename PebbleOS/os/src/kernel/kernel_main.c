/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_main.c
 * @author Evan Stoddard
 * @brief Kernel main thread implementation. Bootstraps core OS services and
 * runs the primary kernel event loop.
 */

#include "kernel_main.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>

#include "pebble_thread.h"

#include "kernel_background.h"
#include "main_event_loop.h"

#include "ble/ble_advertising.h"
#include "ble/ble_conn_mgr.h"

#include "services/services.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kernel_main);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private instance
 */
static struct {
  PebbleThread_t *thread;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Bringing the rest of the threads and services for PebbleOS
 */
static void prv_main_init(void) {
  services_init();

  kernel_background_init();

  ble_conn_mgr_init();
  ble_advertising_begin();
}

/**
 * @brief Kernel main thread entry point. Initializes OS services, then enters
 * the main event loop which runs for the lifetime of the system.
 *
 * @param args Unused thread argument
 */
static void prv_thread_entry(void *args) {
  (void)args;

  LOG_INF("Kernel Main Thread Starting...");
  prv_main_init();
  main_event_loop();
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kernel_main_init(void) {
  prv_inst.thread = pebble_thread_create(PebbleThread_KernelBackground, "Main Thread",
                                         CONFIG_KERNEL_MAIN_STACK_SIZE, CONFIG_KERNEL_MAIN_PRIORITY,
                                         prv_thread_entry, NULL);

  __ASSERT(prv_inst.thread, "Unable to allocate kernel main thread.");

  return 0;
}

PebbleThread_t *kernel_main_thread(void) {
  return prv_inst.thread;
}
