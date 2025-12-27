/*
 * Copyright (C) Ovyl
 */

/**
 * @file ppogatt_protocol_definitions.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ppogatt_protocol_definitions_h
#define ppogatt_protocol_definitions_h

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

typedef enum {
  PPoGATT_PACKET_TYPE_DATA = 0x0,
  PPoGATT_PACKET_TYPE_ACK = 0x1,
  PPoGATT_PACKET_TYPE_RESET = 0x2,
  PPoGATT_PACKET_TYPE_RESET_COMPLETE = 0x3,
} PPoGATT_Packet_Type_t;

/**
 * @typedef PPoGATT_Packet_Header_t
 * @brief [TODO:description]
 *
 */
typedef struct PPoGATT_Packet_Header_t {
  PPoGATT_Packet_Type_t type : 3;
  uint8_t sn : 5;
} __attribute__((__packed__)) PPoGATT_Packet_Header_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_protocol_definitions_h */
