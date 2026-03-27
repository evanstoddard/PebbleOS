/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_remote_version.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef comm_session_remote_version_h
#define comm_session_remote_version_h

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

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Kick off requests for session version
 *
 * @param session Pointer to session instance
 */
void comm_session_remote_version_start_requests(CommSession_t *session);

#ifdef __cplusplus
}
#endif
#endif /* comm_session_remote_version_h */
