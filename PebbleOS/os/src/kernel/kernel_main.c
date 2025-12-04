/*
 * Copyright (C) Ovyl
 */

/**
 * @file kernel_main.c
 * @author Evan Stoddard
 * @brief Kernel main thread
 */

#include "kernel_main.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

#include "events.h"
#include "kernel_background.h"
#include "main_event_loop.h"

#include "services/services.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kernel_main);

/*****************************************************************************
 * Variables
 *****************************************************************************/

static K_KERNEL_STACK_DEFINE(prv_thread_stack, CONFIG_KERNEL_MAIN_STACK_SIZE);

/**
 * @brief Private instance
 */
static struct {
  PebbleThread_t thread;

  bool initialized;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Initialization and setup for rest of the system
 */
static void prv_main_init(void) {

  events_init();

  services_early_init();
  services_init();

  int ret = kernel_background_init();

  if (ret < 0) {
    // TODO: Fatal error handling
    return;
  }
}

/**
 * @brief Kernel main thread entry
 *
 * @param arg Unused args
 */
static void prv_thread_entry(void *arg) {
  prv_main_init();
  main_event_loop();
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kernel_main_init(void) {
  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  int ret = pebble_thread_init(
      &prv_inst.thread, PebbleThread_KernelMain, "Kernel Main",
      prv_thread_stack, CONFIG_KERNEL_MAIN_STACK_SIZE, prv_thread_entry, NULL);

  if (ret < 0) {
    LOG_ERR("Failed to initialize kernel main thread: %d", ret);
    return ret;
  }

  prv_inst.initialized = true;

  return 0;
}

PebbleThread_t *kernel_main_thread(void) {
  if (prv_inst.initialized == false) {
    return NULL;
  }

  return &prv_inst.thread;
}
