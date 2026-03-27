/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_default_kernel_sender.c
 * @author Evan Stoddard
 * @brief
 */

#include <string.h>

#include <zephyr/sys/byteorder.h>

#include "comm_session_send_buffer.h"
#include "comm_session_protocol.h"
#include "comm_session_send_queue.h"

#include "kernel/kernel_heap.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct SendBuffer_t {
  union {
    CommSession_t *session;
    CommSessionSendQueueJob_t *job;
  };

  size_t payload_buffer_length;

  size_t written_length;

  size_t consumed_length;

  struct {
    //! The remainder of this struct is the Pebble Protocol message (header + payload):
    PebbleProtocolHeader_t header;
    uint8_t payload[];
  } __attribute__((__packed__));
} SendBuffer_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Returns remaining length (unconsumed bytes) of send buffer
 *
 * @param send_buffer Pointer of send buffer
 * @return Remaining length of send buffer
 */
static size_t prv_get_remaining_length_bytes(const SendBuffer_t *send_buffer) {
  return (sizeof(PebbleProtocolHeader_t) + send_buffer->written_length -
          send_buffer->consumed_length);
}

/**
 * @brief Get current pointer of read buffer
 *
 * @param send_buffer Pointer to send buffer
 * @return Returns pointer of read buffer
 */
static const uint8_t *prv_get_read_pointer(const SendBuffer_t *send_buffer) {
  return ((const uint8_t *)&send_buffer->header + send_buffer->consumed_length);
}

/**
 * @brief Deallocates send buffer
 *
 * @param send_buffer Send buffer
 */
static void prv_destroy_send_buffer(SendBuffer_t *send_buffer) {
  kernel_heap_free(send_buffer);
}

/*****************************************************************************
 * Send Queue Implementation
 *****************************************************************************/

/**
 * @brief Get length of message in bytes
 *
 * @param job [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_queue_impl_get_length(const CommSessionSendQueueJob_t *job) {
  return prv_get_remaining_length_bytes((SendBuffer_t *)job);
}

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param start_offset [TODO:parameter]
 * @param length [TODO:parameter]
 * @param data_out [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_queue_impl_copy(const CommSessionSendQueueJob_t *send_job, int start_offset,
                                  size_t length, uint8_t *data_out) {
  SendBuffer_t *send_buffer = (SendBuffer_t *)send_job;

  const size_t length_remaining = prv_get_remaining_length_bytes(send_buffer);
  const size_t length_after_offset = (length_remaining - start_offset);
  const size_t length_to_copy = MIN(length_after_offset, length);

  memcpy(data_out, prv_get_read_pointer(send_buffer) + start_offset, length_to_copy);

  return length_to_copy;
}

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param data_out [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_queue_impl_get_read_pointer(const CommSessionSendQueueJob_t *send_job,
                                              const uint8_t **data_out) {
  SendBuffer_t *send_buffer = (SendBuffer_t *)send_job;

  *data_out = prv_get_read_pointer(send_buffer);

  return prv_get_remaining_length_bytes(send_buffer);
}

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param length [TODO:parameter]
 */
static void prv_queue_impl_consume(const CommSessionSendQueueJob_t *send_job, size_t length) {
  SendBuffer_t *send_buffer = (SendBuffer_t *)send_job;
  send_buffer->consumed_length += length;
}

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 */
static void prv_queue_impl_free(CommSessionSendQueueJob_t *send_job) {
  prv_destroy_send_buffer((SendBuffer_t *)send_job);
}

static CommSessionSendJobImpl_t prv_queue_impl = {
    .get_length = prv_queue_impl_get_length,
    .copy = prv_queue_impl_copy,
    .get_read_pointer = prv_queue_impl_get_read_pointer,
    .consume = prv_queue_impl_consume,
    .free = prv_queue_impl_free};

/*****************************************************************************
 * Functions
 *****************************************************************************/

SendBuffer_t *comm_session_send_buffer_begin_write(CommSession_t *session, uint16_t endpoint_id,
                                                   size_t required_free_length,
                                                   uint32_t timeout_ms) {
  // FIXME: Missing a bunch of checks

  // FIXME: Also, doesn't block.  Consider using net_buf_t.  Benefit, not using heap.  Trade-off, is
  // a maximum number of buffers must be determined, and that amount of RAM must be pre-allocated...

  SendBuffer_t *buffer = kernel_heap_malloc(sizeof(SendBuffer_t) + required_free_length);

  if (buffer == NULL) {
    return NULL;
  }

  memset(buffer, 0, sizeof(SendBuffer_t) + required_free_length);

  buffer->session = session;

  buffer->payload_buffer_length = required_free_length;
  buffer->header.endpoint_id = sys_cpu_to_be16(endpoint_id);

  return buffer;
}

bool comm_session_send_buffer_write(SendBuffer_t *send_buffer, const uint8_t *data, size_t length) {
  if (send_buffer == NULL || data == NULL) {
    return false;
  }

  if ((send_buffer->payload_buffer_length - send_buffer->written_length) < length) {
    return false;
  }

  memcpy(send_buffer->payload + send_buffer->header.length + send_buffer->written_length, data,
         length);
  send_buffer->written_length += length;

  return true;
}

void comm_session_send_buffer_end_write(SendBuffer_t *send_buffer) {
  CommSession_t *session = send_buffer->session;

  send_buffer->job->impl = &prv_queue_impl;

  send_buffer->header.length = sys_cpu_to_be16(send_buffer->written_length);

  comm_session_send_queue_add_job(session, (CommSessionSendQueueJob_t *)send_buffer);
}
