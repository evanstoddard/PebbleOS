/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_remote_version.c
 * @author Evan Stoddard
 * @brief
 */

#include <zephyr/logging/log.h>

#include "kernel/main_event_loop.h"

#include "comm_session.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(comm_session_remote_version);

#define COMM_SESSION_REMOTE_VERSION_ENDPOINT_ID (0x0011u)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  CommSessionVersionCommandRequest = 0x00,
  CommSessionVersionCommandResponse = 0x01,
} CommSessionVersionCommand;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Callback from kernel thread to perform version request
 *
 * @param data Pointer to comm session instance
 */
static void prv_perform_version_request(void *data) {
  LOG_INF("Performing version request...");
  CommSession_t *session = (CommSession_t *)data;

  const uint8_t command = CommSessionVersionCommandRequest;

  comm_session_send_data(session, COMM_SESSION_REMOTE_VERSION_ENDPOINT_ID, &command,
                         sizeof(command), COMM_SESSION_DEFAULT_TIMEOUT);
}

/**
 * @brief Schedules version request to be performed by kernel thread
 *
 * @param session Pointer to session instance
 */
static void prv_schedule_request(CommSession_t *session) {
  main_event_loop_add_callback_event(prv_perform_version_request, session);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void comm_session_remote_version_start_requests(CommSession_t *session) {
  prv_schedule_request(session);
}
