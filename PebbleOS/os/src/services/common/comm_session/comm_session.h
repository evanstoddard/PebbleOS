/*
 * Copyright (C) Ovyl
 */

/**
 * @file comm_session.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef comm_session_h
#define comm_session_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define COMM_SESSION_DEFAULT_TIMEOUT_MS (4000U)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef CommSession_t
 * @brief Forward declaration of comm session
 *
 */
typedef struct CommSession_t CommSession_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
void comm_session_init(void);

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @param endpoint_id [TODO:parameter]
 * @param data [TODO:parameter]
 * @param length [TODO:parameter]
 * @param timeout_ms [TODO:parameter]
 * @return [TODO:return]
 */
bool comm_session_send_data(CommSession_t *session, uint16_t endpoint_id, const uint8_t *data,
                            size_t length, uint32_t timeout_ms);

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 * @return [TODO:return]
 */
bool comm_session_default_send_job(CommSession_t *session);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_h */
