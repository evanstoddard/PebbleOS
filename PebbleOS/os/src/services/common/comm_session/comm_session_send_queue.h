/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_send_queue.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef comm_session_send_queue_h
#define comm_session_send_queue_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

typedef struct CommSession_t CommSession_t;

typedef struct CommSessionSendQueueJob_t CommSessionSendQueueJob_t;

typedef struct CommSessionSendJobImpl_t {
  //! @return The size of the message(s) of this job in bytes.
  size_t (*get_length)(const CommSessionSendQueueJob_t *send_job);

  //! Copies bytes from the message(s) into another buffer.
  //! @param start_off The offset into the send buffer
  //! @param length The number of bytes to copy
  //! @param[out] data_out Pointer to the buffer into which to copy the data
  //! @return The number of bytes copied
  //! @note The caller will ensure there is enough data available.
  size_t (*copy)(const CommSessionSendQueueJob_t *send_job, int start_offset, size_t length,
                 uint8_t *data_out);

  //! Gets a read pointer and the number of bytes that can be read from the read pointer.
  //! @note The implementation might use a non-contiguous buffer, so it is possible
  //! that there is more data to read. To access the entire message data, call this function
  //! and consume() repeatedly until it returns zero.
  //! @param data_out Pointer to the pointer to assign the read pointer to.
  //! @return The number of bytes that can be read starting at the read pointer.
  size_t (*get_read_pointer)(const CommSessionSendQueueJob_t *send_job, const uint8_t **data_out);

  //! Indicates that `length` bytes have been consumed and sent out by the transport.
  void (*consume)(const CommSessionSendQueueJob_t *send_job, size_t length);

  //! Called when the send queue is done consuming the job, or when the session is disconnected
  //! and the job should clean itself up.
  void (*free)(CommSessionSendQueueJob_t *send_job);
} CommSessionSendJobImpl_t;

typedef struct CommSessionSendQueueJob_t {
  sys_snode_t node;
  CommSessionSendJobImpl_t *impl;
} CommSessionSendQueueJob_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Queue up job for transmission
 *
 * @param session Pointer to session
 * @param job Pointer to job
 */
void comm_session_send_queue_add_job(CommSession_t *session, CommSessionSendQueueJob_t *job);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_send_queue_h */
