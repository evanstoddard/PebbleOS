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

size_t comm_session_send_queue_copy(CommSession_t *session, uint32_t start_offset, size_t length,
                                    uint8_t *data_out) {
  size_t remaining_length = length;

  const SessionSendQueueJob_t *job = (SessionSendQueueJob_t *)sys_slist_get(&session->send_queue);

  while (job && remaining_length) {
    const size_t job_length = job->impl->get_length(job);

    if (job_length <= start_offset) {
      start_offset -= job_length;
    } else {
      const size_t copied_length = job->impl->copy(job, start_offset, remaining_length, data_out);
      remaining_length -= copied_length;
      data_out += copied_length;
      start_offset = 0;
    }

    job = (SessionSendQueueJob_t *)sys_slist_get(&session->send_queue);
  }

  return (length - remaining_length);
}

size_t comm_session_send_queue_get_length(const CommSession_t *session) {
  return sys_slist_len(&session->send_queue);
}
