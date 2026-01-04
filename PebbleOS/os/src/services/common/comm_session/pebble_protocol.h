/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_protocol.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef pebble_protocol_h
#define pebble_protocol_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

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
#endif /* pebble_protocol_h */
