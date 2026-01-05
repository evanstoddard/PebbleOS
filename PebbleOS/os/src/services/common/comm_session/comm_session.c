/*
 * Copyright (C) Ovyl
 */

/**
 * @file comm_session.c
 * @author Evan Stoddard
 * @brief
 */

#include "comm_session.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/sys/slist.h>

#include "kernel/main_event_loop.h"

#include "session_internal.h"
#include "session_send_buffer.h"
#include "session_remote_version.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(comm_session);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static struct {
  sys_slist_t sessions;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @param is_callback [TODO:parameter]
 */
static void prv_send_next(CommSession_t *session, bool is_callback) {
  if (is_callback) {
    session->is_send_scheduled = false;
  }

  session->implementation->send_next(session->transport);
}

/**
 * @brief [TODO:description]
 *
 * @param data [TODO:parameter]
 */
static void prv_default_send_next_cb(void *data) {}

/*****************************************************************************
 * Functions
 *****************************************************************************/

CommSession_t *comm_session_open(Transport *transport,
                                 const TransportImplementation_t *implementation,
                                 Transport_Destination_t destination) {
  const bool is_system = (destination != TRANSPORT_APP);

  CommSession_t *session = k_malloc(sizeof(CommSession_t));
  if (session == NULL) {
    LOG_ERR("Failed to create new Comm Session.");
    return NULL;
  }

  session->transport = transport;
  session->implementation = implementation;
  session->destination = destination;

  sys_slist_init(&session->send_queue);

  sys_slist_prepend(&prv_inst.sessions, (sys_snode_t *)session);

  session_remote_version_start_requests(session);

  return session;
}

void comm_session_init(void) {
  sys_slist_init(&prv_inst.sessions);
}

bool comm_session_send_data(CommSession_t *session, uint16_t endpoint_id, const uint8_t *data,
                            size_t length, uint32_t timeout_ms) {
  if (session == NULL) {
    return false;
  }

  SendBuffer_t *sb = comm_session_send_buffer_begin_write(session, endpoint_id, length, timeout_ms);
  if (sb == NULL) {
    LOG_ERR("Failed to acquire send buffer.");
    return false;
  }

  LOG_INF("Writing buffer...");

  comm_session_send_buffer_write(sb, data, length);
  comm_session_send_buffer_end_write(sb);
  return true;
}

void comm_session_send_next(CommSession_t *session) {
  LOG_INF("Attempting to queue message...");

  if (session->implementation->schedule == NULL) {
    main_event_loop_add_callback_event(prv_default_send_next_cb, session);
    return;
  }

  if (session->implementation->schedule(session)) {
    LOG_ERR("Failed to schedule outbound message.");
    session->is_send_scheduled = true;
  }

  LOG_INF("Queued outbound message...");
}
