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

#include "session_internal.h"

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
 * Prototypes
 *****************************************************************************/

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

  sys_slist_prepend(&prv_inst.sessions, (sys_snode_t *)session);

  session_remote_version_start_requests(session);

  return session;
}

void comm_session_init(void) {
  sys_slist_init(&prv_inst.sessions);
}
