/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file main_event_loop.c
 * @author Evan Stoddard
 * @brief Main event loop implementation. Dequeues PebbleOS events and
 * dispatches them to the appropriate handler. Currently handles callback
 * events by invoking the stored function pointer directly on the kernel
 * main thread.
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
 * Private Functions
 *****************************************************************************/

/**
 * @brief Dispatch a single dequeued event. Handles callback events by invoking
 * the stored function pointer; all other event types are currently ignored.
 *
 * @param event Pointer to the event to dispatch
 */
static void prv_minimal_event_handler(PebbleEvent_t *event) {
  switch (event->event_type) {
    case PEBBLE_CALLBACK_EVENT:
      event->callback.callback(event->callback.data);
      break;
    default:
      break;
  }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void main_event_loop(void) {
  static PebbleEvent_t event = {};

  while (true) {
    if (events_take(&event, CONFIG_MAIN_EVENT_LOOP_TAKE_TIMEOUT_MS) != 0) {
      continue;
    }

    prv_minimal_event_handler(&event);
  }
}

void main_event_loop_add_callback_event(PebbleCallbackFunction_t callback, void *data) {
  if (callback == NULL) {
    return;
  }

  PebbleEvent_t event = {
      .event_type = PEBBLE_CALLBACK_EVENT, .callback.callback = callback, .callback.data = data};

  events_put_event(&event);
}
