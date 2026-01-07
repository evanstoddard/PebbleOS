/*
 * Copyright (C) Ovyl
 */

/**
 * @file ppogatt_client.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ppogatt_client_h
#define ppogatt_client_h

#include "services/common/comm_session/session_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
void ppogatt_client_init(void);

/**
 * @brief [TODO:description]
 *
 * @param transport [TODO:parameter]
 */
void ppogatt_client_send_next(struct Transport *transport);

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_client_h */
