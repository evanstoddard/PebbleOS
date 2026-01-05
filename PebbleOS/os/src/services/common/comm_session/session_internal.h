/*
 * Copyright (C) Ovyl
 */

/**
 * @file session_internal.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef session_internal_h
#define session_internal_h

#include <stdbool.h>

#include "session_transport.h"

#include <zephyr/sys/slist.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct CommSession_t {
  sys_snode_t node;

  Transport *transport;

  const TransportImplementation_t *implementation;

  Transport_Destination_t destination;

  sys_slist_t send_queue;

  bool is_send_scheduled;
} CommSession_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* session_internal_h */
