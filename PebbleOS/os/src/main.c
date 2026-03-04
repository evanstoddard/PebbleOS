/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file main.c
 * @brief Zephyr application entry point. Initializes the kernel heap and
 * launches the kernel main thread.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "kernel/kernel_heap.h"

#include "kernel/kernel_main.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(main);

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
  LOG_INF("Booting PebbleOS.");

  // Initialize kernel heap
  kernel_heap_init();

  kernel_main_init();

  return 0;
}
