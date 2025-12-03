/*
 * Copyright (C) Ovyl
 */

/**
 * @file kernel_background.c
 * @author Evan Stoddard
 * @brief
 */

#include "kernel_background.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kernel_bg);

/*****************************************************************************
 * Variables
 *****************************************************************************/

K_KERNEL_STACK_DEFINE(prv_thread_stack, CONFIG_KERNEL_BACKGROUND_STACK_SIZE);

static struct {
  PebbleThread_t thread;

  bool initialized;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Kernel background thread entry
 *
 * @param arg Unused
 */
static void prv_thread_entry(void *arg) {
  while (true) {
    k_sleep(K_FOREVER);
  }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kernel_background_init(void) {
  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  int ret = pebble_thread_init(&prv_inst.thread, PebbleThread_KernelBackground,
                               "Kernel BG", prv_thread_stack,
                               CONFIG_KERNEL_BACKGROUND_STACK_SIZE,
                               prv_thread_entry, NULL);

  if (ret < 0) {
    LOG_ERR("Failed to initialize kernel background thread: %d", ret);
    return ret;
  }

  prv_inst.initialized = true;

  return 0;
}

PebbleThread_t *kernel_background_thread(void) {
  if (prv_inst.initialized == false) {
    return NULL;
  }

  return &prv_inst.thread;
}
