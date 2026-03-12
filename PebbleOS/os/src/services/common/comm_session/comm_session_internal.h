/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_internal.h
 * @author Evan Stoddard
 * @brief Internal definitions for comm session
 */

#ifndef comm_session_internal_h
#define comm_session_internal_h

#include <stdint.h>
#include <stddef.h>

#include <zephyr/sys/slist.h>

#include "comm_transport.h"

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
 * @typedef CommSession
 * @brief Actual definition of CommSession_t
 *
 */
typedef struct CommSession_t {
  sys_snode_t node;

  CommTransport_t *transport;

  const CommTransportImpl_t *transport_imp;

  CommTransportDestination_t destination;

} CommSession_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* comm_session_internal_h */
