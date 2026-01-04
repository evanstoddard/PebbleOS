/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_remote_version.c
 * @author Evan Stoddard
 * @brief
 */

#include "session_remote_version.h"

#include <zephyr/logging/log.h>

#include "kernel/main_event_loop.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(session_remote_version);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param data [TODO:parameter]
 */
static void prv_perform_version_request(void *data) {
  CommSession_t *session = (CommSession_t *)data;

  LOG_INF("Performing version request in main thread.");
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void session_remote_version_start_requests(CommSession_t *session) {
  if (session == NULL) {
    return;
  }

  main_event_loop_add_callback_event(prv_perform_version_request, session);
}
