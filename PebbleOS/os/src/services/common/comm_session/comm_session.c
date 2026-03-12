/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session.c
 * @author Evan Stoddard
 * @brief
 */

#include "comm_session.h"

#include <zephyr/logging/log.h>

#include <zephyr/sys/slist.h>

#include "comm_transport.h"
#include "comm_session_internal.h"

#include "kernel/kernel_heap.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(comm_session);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private instance
 */
static struct {
  sys_slist_t sessions;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void comm_session_init(void) {
  sys_slist_init(&prv_inst.sessions);
}

CommSession_t *comm_session_get_system_session(void) {
  // TODO: Actually do this...
  return NULL;
}

/*****************************************************************************
 * Public Transport Functions
 *****************************************************************************/

CommSession_t *comm_session_open(CommTransport_t *transport,
                                 const CommTransportImpl_t *implementation,
                                 CommTransportDestination_t destination) {
  const bool is_system = (destination != CommTransportDestinationApp);

  if (is_system == true) {
    CommSession_t *existing_system_session = comm_session_get_system_session();

    if (existing_system_session) {
      LOG_INF("System session already exists, closing it now");
      existing_system_session->transport_imp->close(existing_system_session->transport);
    }
  }

  CommSession_t *session = kernel_heap_malloc(sizeof(CommSession_t));

  if (!session) {
    LOG_ERR("Not enough memory for new Comm Session");
    return NULL;
  }

  *session = (const CommSession_t){
      .transport = transport,
      .transport_imp = implementation,
      .destination = destination,
  };

  sys_slist_prepend(&prv_inst.sessions, (sys_snode_t *)session);

  // TODO: Kick off requests

  // TODO: Send kernel event

  LOG_INF("Session created!");

  return session;
}

void comm_session_close(CommSession_t *session, CommSessionCloseReason_t reason) {
  // TODO: A lot of this is missing, but I allocated the memory, so I better clean it up...
  sys_slist_find_and_remove(&prv_inst.sessions, (sys_snode_t *)session);

  kernel_heap_free(session);
}

void comm_session_receive_router_write(CommSession_t *session, const uint8_t *data,
                                       size_t data_size) {
  // TODO: Implement this
}

size_t comm_session_send_queue_get_length(const CommSession_t *session) {
  // TODO: Implement this
  return 0;
}

size_t comm_session_send_queue_copy(CommSession_t *session, uint32_t start_offset, size_t length,
                                    uint8_t *data_out) {
  // TODO: Implement this
  return 0;
}

size_t comm_session_send_queue_get_read_pointer(const CommSession_t *session,
                                                const uint8_t **data_out) {
  // TODO: Implement this
  return 0;
}

void comm_session_send_queue_consume(CommSession_t *session, size_t length) {}

void comm_session_send_next(CommSession_t *session) {}
