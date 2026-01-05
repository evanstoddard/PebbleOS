/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_send_queue.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef session_send_queue_h
#define session_send_queue_h

#include <stdint.h>
#include <stddef.h>

#include "comm_session.h"

#include <zephyr/sys/slist.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef SessionSendQueueJob_t
 * @brief Forward declaration of session send queue job
 *
 */
typedef struct SessionSendQueueJob_t SessionSendQueueJob_t;

/**
 * @typedef SessionSendQueueJobImpl_t
 * @brief [TODO:description]
 *
 */
typedef struct SessionSendQueueJobImpl_t {
  size_t (*get_length)(const SessionSendQueueJob_t *send_job);

  size_t (*copy)(const SessionSendQueueJob_t *send_job, int start_offset, size_t length,
                 uint8_t *data_out);

  size_t (*get_read_pointer)(const SessionSendQueueJob_t *send_job, const uint8_t **data_out);

  void (*consume)(const SessionSendQueueJob_t *send_job, size_t length);

  void (*free)(SessionSendQueueJob_t *send_job);

} SessionSendQueueJobImpl_t;

/**
 * @typedef SessionSendQueueJob_t
 * @brief [TODO:description]
 *
 */
typedef struct SessionSendQueueJob_t {
  sys_snode_t node;
  const SessionSendQueueJobImpl_t *impl;
} SessionSendQueueJob_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @param job [TODO:parameter]
 */
void comm_session_send_queue_add_job(CommSession_t *session, SessionSendQueueJob_t **job);

#ifdef __cplusplus
}
#endif
#endif /* session_send_queue_h */
