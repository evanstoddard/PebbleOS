/*
 * Copyright (C) Ovyl
 */

/**
 * @file main_event_loop.c
 * @author Evan Stoddard
 * @brief
 */

#include "main_event_loop.h"

#include <zephyr/logging/log.h>

#include "events.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(main_event_loop);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void main_event_loop(void) {
  static PebbleEvent_t event = {};

  while (true) {

    if (events_take(&event, CONFIG_MAIN_EVENT_LOOP_TAKE_TIMEOUT_MS) != 0) {
      continue;
    }

    LOG_INF("Received event: 0x%02X", event.event_type);
  }
}
