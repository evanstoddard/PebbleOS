/*
 * Copyright (C) Ovyl
 */

/**
 * @file main.c
 * @brief
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "kernel/kernel_main.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(void) {
  LOG_INF("PebbleOS Booting...");

  int ret = kernel_main_init();

  if (ret < 0) {
    LOG_ERR("Failed to initialize kernel main thread: %d", ret);
    // TODO: Fatal error handling

    return ret;
  }

  return 0;
}
