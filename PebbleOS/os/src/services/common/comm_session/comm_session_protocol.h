/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file comm_session_protocol.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef comm_session_protocol_h
#define comm_session_protocol_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define COMM_PRIVATE_MAX_INBOUND_PAYLOAD_SIZE 2044
#define COMM_PUBLIC_MAX_INBOUND_PAYLOAD_SIZE 144

// TODO: If we have memory to spare, let's crank this up to improve data spooling
#define COMM_MAX_OUTBOUND_PAYLOAD_SIZE 656

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct PebbleProtocolHeader_t {
  uint16_t length;
  uint16_t endpoint_id;
} __attribute__((__packed__)) PebbleProtocolHeader_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* comm_session_protocol_h */
