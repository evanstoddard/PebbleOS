/*
 * Copyright (C) Ovyl
 */

/**
 * @file comm_default_kernel_sender.c
 * @author Evan Stoddard
 * @brief
 */

/* NOTE: Initially looked into using the zephyr net buffer library, but that library has a compile
 * time upper limit on number of buffers that can be allocated from a bool, rather than allowing an
 * arbitrary amount of buffers, as long as the total allocated space is <= pool size.  So back to
 * manually handling this... */

#include "comm_default_kernel_sender.h"

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>

#include "pebble_protocol.h"

#include "session_internal.h"
#include "session_send_queue.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define COMM_SENDER_MAX_ALLOCATED \
  (sizeof(PebbleProtocolHeader_t) + CONFIG_MAX_DEFAULT_KERNEL_SENDER_PAYLOAD_SIZE)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef SendBuffer_t
 * @brief [TODO:description]
 *
 */
typedef struct SendBuffer_t {
  union {
    CommSession_t *sesson;
    SessionSendQueueJob_t *job;
  };

  size_t payload_buf_len_bytes;
  size_t written_len_bytes;
  size_t consumed_len_bytes;

  struct {
    PebbleProtocolHeader_t header;
    uint8_t payload[];
  } __attribute__((__packed__));
} SendBuffer_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/* NOTE: Original implementation did not dedicate specific heap and therefore could effectively
ignore the size of SendBuffer_t.  For speed of implementing MVP, dedicated a heap so I wouldn't need
to implement the signaling for allocation availability.  Should revist this. */
K_HEAP_DEFINE(prv_buf_heap, (sizeof(SendBuffer_t) + COMM_SENDER_MAX_ALLOCATED));

/*****************************************************************************
 * Session Send Queue Job Implementation Declarations
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_job_get_len(const SessionSendQueueJob_t *send_job);

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param start_offset [TODO:parameter]
 * @param length [TODO:parameter]
 * @param data_out [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_job_copy(const SessionSendQueueJob_t *send_job, int start_offset, size_t length,
                           uint8_t *data_out);

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param data_out [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_job_get_read_pointer(const SessionSendQueueJob_t *send_job,
                                       const uint8_t **data_out);

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 * @param length [TODO:parameter]
 */
static void prv_job_consume(const SessionSendQueueJob_t *send_job, size_t length);

/**
 * @brief [TODO:description]
 *
 * @param send_job [TODO:parameter]
 */
static void prv_job_free(SessionSendQueueJob_t *send_job);

/**
 * @brief [TODO:description]
 */
static SessionSendQueueJobImpl_t prv_send_queue_job_impl = {
    .get_length = prv_job_get_len,
    .copy = prv_job_copy,
    .get_read_pointer = prv_job_get_read_pointer,
    .consume = prv_job_consume,
    .free = prv_job_free};

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @param payload_len_bytes [TODO:parameter]
 * @param endpoint_id [TODO:parameter]
 * @param timeout [TODO:parameter]
 * @return [TODO:return]
 */
SendBuffer_t *prv_send_buffer_create(CommSession_t *session, size_t payload_len_bytes,
                                     uint16_t endpoint_id, k_timeout_t timeout) {
  /* NOTE: Original implementation asserted the bt_lock was held.  Curious if that is needed here...
   * Will revisit locking across entire comm session subsystem later... famous last words... */

  size_t alloc_size = sizeof(SendBuffer_t) + payload_len_bytes;

  SendBuffer_t *buffer = (SendBuffer_t *)k_heap_alloc(&prv_buf_heap, alloc_size, timeout);

  if (buffer == NULL) {
    return NULL;
  }

  memset(buffer, 0, alloc_size);

  buffer->sesson = session;
  buffer->payload_buf_len_bytes = payload_len_bytes;

  buffer->header.endpoint_id = sys_cpu_to_be16(endpoint_id);

  return buffer;
}

/**
 * @brief [TODO:description]
 *
 * @param buffer [TODO:parameter]
 */
void prv_send_buffer_destroy(SendBuffer_t *buffer) {
  k_heap_free(&prv_buf_heap, buffer);
}

/*****************************************************************************
 * Send Job Implementation
 *****************************************************************************/

static size_t prv_job_get_len(const SessionSendQueueJob_t *send_job) {
  return 0;
}

static size_t prv_job_copy(const SessionSendQueueJob_t *send_job, int start_offset, size_t length,
                           uint8_t *data_out) {
  return 0;
}

static size_t prv_job_get_read_pointer(const SessionSendQueueJob_t *send_job,
                                       const uint8_t **data_out) {
  return 0;
}

static void prv_job_consume(const SessionSendQueueJob_t *send_job, size_t length) {}

static void prv_job_free(SessionSendQueueJob_t *send_job) {}

/*****************************************************************************
 * Functions
 *****************************************************************************/
