/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_remote_version.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef session_remote_version_h
#define session_remote_version_h

#include "session_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param session [TODO:parameter]
 */
void session_remote_version_start_requests(CommSession_t *session);

#ifdef __cplusplus
}
#endif
#endif /* session_remote_version_h */
