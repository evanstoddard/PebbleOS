/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file ppogatt_client_internal.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ppogatt_client_internal_h
#define ppogatt_client_internal_h

#include <stdint.h>

#include <zephyr/bluetooth/gatt.h>

#include <zephyr/sys/slist.h>
#include <zephyr/sys/uuid.h>

#include "services/common/comm_session/comm_session.h"
#include "services/common/comm_session/comm_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/
#define PPOGATT_V1_DESIRED_RX_WINDOW_SIZE (4500 / 251)
#define PPOGATT_MIN_VERSION (0x00)
#define PPOGATT_MAX_VERSION (0x01)
#define PPOGATT_SN_BITS (5)
#define PPOGATT_SN_MOD_DIV (1 << PPOGATT_SN_BITS)
#define PPOGATT_V0_WINDOW_SIZE (4)
#define PPOGATT_TIMEOUT_TICK_INTERVAL_SECS (2)

//! Effective timeout: between 5 - 6 secs, because packet could be sent out just before the
//! RegularTimer second tick is about to fire.
#define PPOGATT_TIMEOUT_TICKS (3)

//! Number of maximum consecutive timeouts without getting a packet Ack'd
#define PPOGATT_TIMEOUT_COUNT_MAX (2)

//! Number of maximum consecutive resets without getting a packet Ack'd
#define PPOGATT_RESET_COUNT_MAX (5)

//! Number of maximum consecutive disconnects without getting a packet Ack'd
#define PPOGATT_DISCONNECT_COUNT_MAX (2)

//! Maximum amount of time PPoGATT will wait before sending an Ack for received data
#define PPOGATT_MAX_DATA_ACK_LATENCY_MS (200)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  PPoGATTPacketTypeData = 0x0,
  PPoGATTPacketTypeAck = 0x1,
  PPoGATTPacketTypeResetRequest = 0x2,
  PPoGATTPacketTypeResetComplete = 0x3,
  PPoGATTPacketTypeInvalidRangeStart,
} PPoGATTPacketType_t;

typedef enum {
  PPoGATTStateDisconnectedReadingMeta,
  PPoGATTStateDisconnectedSubscribingData,
  // StateConnectedClosedAwaitingResetRequest, // Server-only state
  PPoGATTStateConnectedClosedAwaitingResetCompleteSelfInitiatedReset,
  PPoGATTStateConnectedClosedAwaitingResetCompleteSelfInitiatedResetStalled,
  PPoGATTStateConnectedClosedAwaitingResetCompleteRemoteInitiatedReset,
  PPoGATTStateConnectedOpen,
} PPoGATTClientState_t;

typedef enum {
  PPoGATTAckTimeoutState_Inactive = 0,
  PPoGATTAckTimeoutState_Active = 1,
  PPoGATTAckTimeoutState_TimedOut = PPoGATTAckTimeoutState_Active + PPOGATT_TIMEOUT_TICKS,
} PPoGATTAckTimeoutState_t;

/**
 * @typedef PPoGATTPacketHeader_t
 * @brief Header for PPoGATT packet header
 *
 */
typedef struct PPoGATTPacketHeader_t {
  PPoGATTPacketType_t type : 3;
  uint8_t sn : PPOGATT_SN_BITS;
} __attribute__((__packed__)) PPoGATTPacketHeader_t;

/**
 * @typedef PPoGATTClientResetRequest_t
 * @brief PPoGATT reset request from client
 *
 */
typedef struct PPoGATTClientResetRequest_t {
  PPoGATTPacketHeader_t header;
  uint8_t ppogatt_version;
  char serial_number[CONFIG_MFG_SERIAL_NUMBER_SIZE];
} __attribute__((__packed__)) PPoGATTClientResetRequest_t;

/**
 * @typedef PPoGATTResetComplete_t
 * @brief PPoGATT reset complete packet
 *
 */
typedef struct PPoGATTResetComplete_t {
  uint8_t ppogatt_max_rx_window;
  uint8_t ppogatt_max_tx_window;
} __attribute__((__packed__)) PPoGATTResetComplete_t;

/**
 * @typedef PPoGATTDataPacket_t
 * @brief PPoGATT data packet;
 *
 */
typedef struct PPoGATTDataPacket_t {
  PPoGATTPacketHeader_t header;
  uint8_t payload[];
} __attribute__((__packed__)) PPoGATTDataPacket_t;

typedef struct PPoGATTMetaV0_t {
  uint8_t ppogatt_min_version;
  uint8_t ppogatt_max_version;
  struct uuid app_uuid;
} __attribute__((__packed__)) PPoGATTMetaV0_t;

typedef enum PPoGATTSessionType_t {
  PPoGATTSessionType_InferredFromUuid = 0x00,
  PPoGATTSessionType_Hybrid = 0x01,
  PPoGATTSessionTypeCount,
} __attribute__((__packed__)) PPoGATTSessionType_t;

typedef struct PPoGATTMetaV1_t {
  uint8_t ppogatt_min_version;
  uint8_t ppogatt_max_version;
  struct uuid app_uuid;
  PPoGATTSessionType_t pp_session_type : 8;
} __attribute__((__packed__)) PPoGATTMetaV1_t;

/**
 * @typedef PPoGATT_Client_t
 * @brief Instance of a PPoGATT client
 *
 */
typedef struct PPoGATT_Client_t {
  sys_snode_t node;

  uint16_t meta_handle;
  uint16_t data_handle;
  uint16_t service_end_handle;

  struct bt_gatt_read_params read_params;
  struct bt_gatt_subscribe_params subscribe_params;
  struct bt_gatt_discover_params ccc_disc_params;

  PPoGATTClientState_t state;

  PPoGATTMetaV1_t meta;

  uint8_t resets_counter;
  bool disconnect_requested;

  struct {
    // If not 0, will allocate and populate correct reset packet and send out
    PPoGATTPacketType_t send_reset_packet_and_type;

    // If not 0, will allocate and populate ACK packet and send out
    PPoGATTPacketType_t send_ack_packet;

    uint16_t payload_sizes[PPOGATT_SN_MOD_DIV];
    uint8_t tx_window_size;
    uint8_t rx_window_size;

    PPoGATTAckTimeoutState_t ack_timeout_state;

    //! Number of consecutive timeouts so far
    uint8_t timeouts_counter;

    uint8_t next_expected_ack_sn;
    uint8_t next_data_sn;

    bool send_rx_ack_now;              //! True if we want to flush the Ack immediately!
    uint8_t outstanding_rx_ack_count;  //! Count of how many data packets we have yet to Ack

  } tx_ctx;

  struct {
    uint8_t next_expected_data_sn;
  } rx_ctx;

  CommSession_t *session;
  CommTransportDestination_t destination;

} PPoGATT_Client_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_client_internal_h */
