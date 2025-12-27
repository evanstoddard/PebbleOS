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

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/slist.h>

#include "ppogatt_protocol_definitions.h"

#include "utils/uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define PPOGATT_SN_BITS (5)

/* TODO: I need to understand WTF this is */
#define PPOGATT_SN_MOD_DIV (1 << PPOGATT_SN_BITS)

#define PPOGATT_TIMEOUT_TICKS (3)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  PPoGATT_CLIENT_STATE_DISCONNECTED_READING_META,
  PPoGATT_CLIENT_STATE_DISCONNECTED_SUBSCRIBING_DATA,
  PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE,
  PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE_STALLED,
  PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_REMOTE_RESET_COMPLETE,
  PPoGATT_CLIENT_STATE_CONNECTED_OPEN,
} PPoGATT_Client_State_t;

typedef enum {
  PPoGATT_TRANSPORT_SYSTEM,
  PPoGATT_TRANSPORT_APP,
  PPoGATT_TRANSPORT_HYBRID,
} PPoGATT_Transport_Destination_t;

typedef enum {
  PPoGATT_ACK_TIMEOUT_STATE_INACTIVE = 0,
  PPoGATT_ACK_TIMEOUT_STATE_ACTIVE = 1,

  /* NOTE: I don't know if I'm impressed by this or disgusted.  Only time will
     tell... */
  PPoGATT_ACK_TIMEOUT_STATE_TIMED_OUT = (PPoGATT_ACK_TIMEOUT_STATE_ACTIVE + PPOGATT_TIMEOUT_TICKS)
} PPoGATT_Ack_Timeout_State_t;

/**
 * @typedef PPoGATT_Client_t
 * @brief PPoGATT Client definition
 *
 */
typedef struct PPoGATT_Client_t {
  /*
   * Must be first property!
   */
  sys_snode_t node;

  PPoGATT_Client_State_t state;

  uint8_t version;

  /*
   * Not a huge fan of "app" here as it could be confused with a UUID of an app
   * installed on the pebble device (at least for me).  Also, if the platform is
   * ever expanded to allow other devices to use PPoGATT, then "app" is a
   * misnomer.  Would "remote" be more appropriate?
   */
  Uuid_t app_uuid;

  struct {
    uint16_t meta_handle;
    uint16_t data_handle;
  } ble_handles;

  /*
   * To quote original source:
   * "Stuffs that deals with inbound data"
   */
  struct {
    uint8_t next_expected_sn;
  } recv_ctx;

  /*
   * To quote original source:
   * "Stuffs that deals with outbound data"
   */
  struct {
    /* TODO: This will get refactored */
    union {
      PPoGATT_Packet_Header_t packet;
      uint8_t byte;
    } reset_packet;

    /* TODO: This, too, will get refactored */
    union {
      PPoGATT_Packet_Header_t packet;
      uint8_t byte;
    } ack_packet;

    /* TODO: I need to understand why there are 32 of these... I also need to
     * get smarter */
    uint16_t payload_sizes[PPOGATT_SN_MOD_DIV];
    uint8_t rx_window_size;
    uint8_t tx_window_size;

    PPoGATT_Ack_Timeout_State_t ack_timeout_state;

    /* NOTE: In first grade, we had one of these, and if you got 5 timeouts in a
     * day, a note got sent home to your parents */
    uint8_t timeouts_counter;

    uint8_t next_expected_ack_sn;
    uint8_t next_data_sn;

    bool send_rx_ack_now;
    uint8_t outstanding_rx_ack_count;
  } write_ctx;

  uint8_t resets_counter;

  bool reset_requested;

  struct k_timer rx_ack_counter;

  PPoGATT_Transport_Destination_t transport_dest;

  /* TODO: Create Comm session definition */
  void *comm_session;
} PPoGATT_Client_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize manager for PPoGATT clients
 */
void ppogatt_client_manager_init(void);

/**
 * @brief Deinit and destroy client
 */
void ppogatt_client_manager_destroy_all(void);

/**
 * @brief Allocate and initialize a PPoGATT client
 *
 * @return Pointer to created client
 */
PPoGATT_Client_t *ppogatt_client_create(void);

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
void ppogatt_client_destroy(PPoGATT_Client_t *client);

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_client_h */
