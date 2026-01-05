/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_send_queue.c
 * @author Evan Stoddard
 * @brief
 */

#include "session_send_queue.h"

#include "session_internal.h"

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

void comm_session_send_queue_add_job(CommSession_t *session, SessionSendQueueJob_t **job_ptr_ptr) {
  SessionSendQueueJob_t *job = *job_ptr_ptr;

  sys_slist_append(&session->send_queue, (sys_snode_t *)job);

  comm_session_send_next(session);
}
