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

#include "comm_session.h"
#include "session_send_buffer.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(session_remote_version);

static const uint16_t SESSION_REMOTE_VERSION_ENDPOINT_ID = 0x0011;

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  CommSessionVersionCommandRequest = 0x00,
  CommSessionVersionCommandResponse = 0x01,
} CommSessionVersionCommand_t;

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

  const uint8_t command = CommSessionVersionCommandRequest;
  // No need to check validity of session here, comm_session_send_data already does this
  comm_session_send_data(session, SESSION_REMOTE_VERSION_ENDPOINT_ID, &command, sizeof(command),
                         COMM_SESSION_DEFAULT_TIMEOUT_MS);
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
