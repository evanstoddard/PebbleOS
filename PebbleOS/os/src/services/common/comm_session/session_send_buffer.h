/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_send_buffer.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef session_send_buffer_h
#define session_send_buffer_h

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
 * @typedef SendBuffer_t
 * @brief Forward declaration of send buffer
 *
 */
typedef struct SendBuffer_t SendBuffer_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @param endpoint_id [TODO:parameter]
 * @param required_free_length [TODO:parameter]
 * @param timeout_ms [TODO:parameter]
 * @return [TODO:return]
 */
SendBuffer_t *comm_session_send_buffer_begin_write(CommSession_t *session, uint16_t endpoint_id,
                                                   size_t required_free_length,
                                                   uint32_t timeout_ms);

/**
 * @brief [TODO:description]
 *
 * @param send_buffer [TODO:parameter]
 * @param data [TODO:parameter]
 * @param length [TODO:parameter]
 * @return [TODO:return]
 */
bool comm_session_send_buffer_write(SendBuffer_t *send_buffer, const uint8_t *data, size_t length);

void comm_session_send_buffer_end_write(SendBuffer_t *send_buffer);

#ifdef __cplusplus
}
#endif
#endif /* session_send_buffer_h */
