/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_send_queue.c
 * @author Evan Stoddard
 * @brief
 */

#include "comm_session_internal.h"
#include "comm_session_send_queue.h"

#include <zephyr/sys/slist.h>

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

void comm_session_send_queue_add_job(CommSession_t *session, CommSessionSendQueueJob_t *job) {
  sys_slist_append(&session->send_jobs, (sys_snode_t *)job);

  comm_session_send_next(session);
}
