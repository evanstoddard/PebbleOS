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

#include "utils/uuid.h"

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
 * @typedef PPoGATT_Meta_t
 * @brief [TODO:description]
 *
 */
typedef struct PPoGATT_Meta_t {
  uint8_t min_version;
  uint8_t max_version;
  Uuid_t app_uuid;
  uint8_t session_type;
} __attribute__((__packed__)) PPoGATT_Meta_t;

/**
 * @typedef PPoGATT_Packet_Header_t
 * @brief [TODO:description]
 *
 */
typedef struct PPoGATT_Packet_Header_t {
  PPoGATT_Packet_Type_t type : 3;
  uint8_t sn : 5;
} __attribute__((__packed__)) PPoGATT_Packet_Header_t;

typedef struct PPoGATT_Packet_t {
  PPoGATT_Packet_Header_t header;
  uint8_t payload[];
} __attribute__((__packed__)) PPoGATT_Packet_t;

/**
 * @typedef PPoGATT_Reset_Packet_t
 * @brief [TODO:description]
 *
 */
typedef struct PPoGATT_Reset_Packet_t {
  PPoGATT_Packet_Header_t header;
  uint8_t version;
  char serial_num[12];
} __attribute__((__packed__)) PPoGATT_Reset_Packet_t;

typedef struct PPoGATT_Reset_Complete_Packet_t {
  PPoGATT_Packet_Header_t header;
  uint8_t max_rx_window;
  uint8_t max_tx_window;
} __attribute__((__packed__)) PPoGATT_Reset_Complete_Packet_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_protocol_definitions_h */
