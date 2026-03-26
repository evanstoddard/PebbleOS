/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_send_buffer.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef comm_session_send_buffer_h
#define comm_session_send_buffer_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "comm_session.h"

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
 * @typedef SendBuffer
 * @brief Opaque declaration of send buffer
 *
 */
typedef struct SendBuffer_t SendBuffer_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Max payload size that can be allocated
 *
 * @param session Pointer to session instance
 * @return Maximum size in bytes of payload
 */
size_t comm_session_send_buffer_get_max_payload_length(const CommSession_t *session);

/**
 * @brief Initializes buffer on heap (I have thoughts on this...)
 *
 * @param session Pointer to session instance
 * @param endpoint_id Endpoint to send message to
 * @param required_free_length Minimum required space to allocate
 * @param timeout_ms Time to block waiting for allocation (Currently unused since I'm lazy)
 * @return Returns pointer to buffer (can be NULL)
 */
SendBuffer_t *comm_session_send_buffer_begin_write(CommSession_t *session, uint16_t endpoint_id,
                                                   size_t required_free_length,
                                                   uint32_t timeout_ms);

/**
 * @brief Copies data to send buffer
 *
 * @param send_buffer Pointer to send buffer
 * @param data Data to copy
 * @param length Length of data to copy
 * @return Returns false on failure
 */
bool comm_session_send_buffer_write(SendBuffer_t *send_buffer, const uint8_t *data, size_t length);

/**
 * @brief Complete write to buffer and commit data for transmission
 *
 * @param send_buffer Pointer to send buffer
 */
void comm_session_send_buffer_end_write(SendBuffer_t *send_buffer);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_send_buffer_h */
