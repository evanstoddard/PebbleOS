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

/*****************************************************************************
 * Send Queue Implementation
 *****************************************************************************/

static CommSessionSendJobImpl_t prv_queue_impl = {

};

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

  // TODO: Queue up job
}
