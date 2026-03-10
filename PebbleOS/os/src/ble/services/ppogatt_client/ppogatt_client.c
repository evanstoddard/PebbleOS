/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file ppogatt_client.c
 * @author Evan Stoddard
 * @brief
 */

#include "ppogatt_client.h"

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include <zephyr/sys/slist.h>

#include "ble/ble_conn_mgr.h"
#include "ble/pebble_bt.h"

#include "kernel/kernel_heap.h"

#include "ppogatt_client_internal.h"
#include "zephyr/sys/uuid.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ppogatt_client);

/*****************************************************************************
 * Variables
 *****************************************************************************/

typedef enum {
  PPOGATT_CHAR_DATA,
  PPOGATT_CHAR_META,
  PPOGATT_CHAR_COUNT,
} PPoGATT_Char_Idx_t;

/**
 * @brief UUID for PPoGATT Data Characteristic
 */
static struct bt_uuid_128 prv_ppogatt_data_uuid =
    BT_UUID_INIT_128(PEBBLE_BT_UUID_EXPAND(PEBBLE_BT_PPOGATT_DATA_CHARACTERISTIC_UUID_32BIT));

/**
 * @brief UUID for PPoGATT Meta Characteristic
 */
static struct bt_uuid_128 prv_ppogatt_meta_uuid =
    BT_UUID_INIT_128(PEBBLE_BT_UUID_EXPAND(PEBBLE_BT_PPOGATT_META_CHARACTERISTIC_UUID_32BIT));

/**
 * @brief Array of PPoGATT Characteristic UUIDs
 */
static const struct bt_uuid *prv_ppogatt_char_uuids[PPOGATT_CHAR_COUNT] = {
    [PPOGATT_CHAR_DATA] = (const struct bt_uuid *)&prv_ppogatt_data_uuid,
    [PPOGATT_CHAR_META] = (const struct bt_uuid *)&prv_ppogatt_meta_uuid,
};

/**
 * @brief Private instance
 */
static struct {
  struct bt_conn *conn;

  sys_slist_t clients;

} prv_inst;

/*****************************************************************************
 * Client Lifecycle Management(?)
 *****************************************************************************/

/**
 * @brief Instantiate and initialize new PPoGATT client
 *
 * @return Returns pointer to new client
 */
static PPoGATT_Client_t *prv_create_client(void) {
  PPoGATT_Client_t *client = kernel_heap_malloc(sizeof(PPoGATT_Client_t));

  if (client == NULL) {
    return NULL;
  }

  memset(client, 0, sizeof(PPoGATT_Client_t));

  return client;
}

/**
 * @brief Destroy allocated client
 *
 * @param client Pointer to client
 */
static void prv_destroy_client(PPoGATT_Client_t *client) {
  __ASSERT(client, "Attempted to destroy a NULL PPoGATT client!");

  sys_slist_find_and_remove(&prv_inst.clients, (sys_snode_t *)client);

  kernel_heap_free(client);
}

/**
 * @brief Destroy all allocated clients
 */
static void prv_destroy_all_clients(void) {
  sys_snode_t *tmp;
  sys_snode_t *node;
  SYS_SLIST_FOR_EACH_NODE_SAFE(&prv_inst.clients, node, tmp) {
    PPoGATT_Client_t *client = (PPoGATT_Client_t *)node;

    prv_destroy_client(client);
  }
}

/*****************************************************************************
 * Packet Transmission
 *****************************************************************************/

/**
 * @brief Prepare a reset packet for transmission
 *
 * @param client Pointer to client
 * @param out_size_bytes Pointer to write total packet size to
 * @return Returns pointer to created packet (can be NULL)
 */
static const PPoGATTPacket_t *prv_prepare_reset_packet(PPoGATT_Client_t *client,
                                                       uint16_t *out_size_bytes) {
  PPoGATTPacket_t *packet = NULL;

  __ASSERT((client->tx_ctx.send_reset_packet_and_type == PPoGATTPacketTypeResetRequest ||
            client->tx_ctx.send_reset_packet_and_type == PPoGATTPacketTypeResetComplete),
           "Invalid outbound PPoGATT reset type!");

  if (client->tx_ctx.send_reset_packet_and_type == PPoGATTPacketTypeResetRequest) {
    packet =
        kernel_heap_malloc(sizeof(PPoGATTPacket_t) + sizeof(PPoGATTResetRequestClientIDPayload_t));

    if (packet == NULL) {
      return NULL;
    }

    packet->type = PPoGATTPacketTypeResetRequest;
    packet->sn = 0;

    PPoGATTResetRequestClientIDPayload_t *payload =
        (PPoGATTResetRequestClientIDPayload_t *)packet->payload;
    payload->ppogatt_version = client->meta.ppogatt_max_version;
    memcpy(payload->serial_number, "ABCDEF012345", CONFIG_MFG_SERIAL_NUMBER_SIZE);

    *out_size_bytes = sizeof(PPoGATTPacket_t) + sizeof(PPoGATTResetRequestClientIDPayload_t);

    return packet;
  }

  packet =
      kernel_heap_malloc(sizeof(PPoGATTPacket_t) + sizeof(PPoGATTResetCompleteClientIDPayloadV1_t));

  if (packet == NULL) {
    return NULL;
  }

  packet->type = PPoGATTPacketTypeResetComplete;
  packet->sn = 0;

  // FIXME: Again, assuming only enhanced features are supported...

  PPoGATTResetCompleteClientIDPayloadV1_t *payload =
      (PPoGATTResetCompleteClientIDPayloadV1_t *)packet->payload;
  payload->ppogatt_max_rx_window = client->tx_ctx.rx_window_size;
  payload->ppogatt_max_tx_window = client->tx_ctx.tx_window_size;

  *out_size_bytes = sizeof(PPoGATTPacket_t) + sizeof(PPoGATTResetCompleteClientIDPayloadV1_t);

  return packet;
}

/**
 * @brief Prepare next packet for transmission
 *
 * @param client Pointer to client instance
 * @param out_size_bytes Pointer to write size of packet to
 * @return Returns pointer to next packet (may be NULL)
 */
static const PPoGATTPacket_t *prv_prepare_next_packet(PPoGATT_Client_t *client,
                                                      uint16_t *out_size_bytes) {
  if (client->tx_ctx.send_reset_packet_and_type != 0) {
    LOG_INF("Queued up reset packet for transmission.");
    return prv_prepare_reset_packet(client, out_size_bytes);
  }

  // TODO: Literally, every other packet
  return NULL;
}

/**
 * @brief Finalize packet that was queued up to BLE stack
 *
 * @param client Pointer to client instance
 * @param payload_size_bytes Size of queued packet in bytes
 */
static void prv_finalize_queued_packet(PPoGATT_Client_t *client, uint16_t payload_size_bytes) {
  // Reset "flags" indicating reset packet should be sent on next transmission
  if (client->tx_ctx.send_reset_packet_and_type != 0) {
    client->tx_ctx.send_reset_packet_and_type = 0;
  }
}

/**
 * @brief Send queued up packets
 *
 * @param client Pointer to client instance
 */
static void prv_send_next_packets(PPoGATT_Client_t *client) {
  uint16_t payload_size = 0;
  const PPoGATTPacket_t *packet = NULL;

  // Cap the number of times we loop here, to avoid blocking the task for too long.
  uint8_t loop_count = 0;

  while ((packet = prv_prepare_next_packet(client, &payload_size))) {
    ++loop_count;

    int ret = bt_gatt_write_without_response(prv_inst.conn, client->data_handle, packet,
                                             payload_size, false /*?*/);
    if (ret != 0) {
      LOG_ERR("Failed to queue up packet to BLE stack: %d", ret);
      break;
    }

    prv_finalize_queued_packet(client, payload_size);

    const uint8_t max_loop_count = 10;
    if (loop_count > max_loop_count) {
      // If more bytes left to send (but loop_count became >= 10),
      // schedule a callback to process them later to avoid blocking the task for too long:
      /* prv_send_next_packets_async(client); */
      break;
    }
  }

  // Ok to call free with NULL ptr as heap API handles that gracefully
  kernel_heap_free((void *)packet);
}

/*****************************************************************************
 * Reset Handling
 *****************************************************************************/

/**
 * @brief Enter state awaiting for reset complete
 *
 * @param client Pointer to client instance
 * @param self_initiated If we are the ones initiating reset vs. remote initiating reset
 */
static void prv_enter_awaiting_reset_complete(PPoGATT_Client_t *client, bool self_initiated) {
  // If a session was open, close and let it clean itself up
  if (client->state == PPoGATTStateConnectedOpen) {
    // TODO: The concept of a session current doesn't exist... but when it does... oh boy... get
    // ready.
  }

  // Clear contexts
  client->rx_ctx.next_expected_data_sn = 0;
  memset(&client->tx_ctx, 0, sizeof(client->tx_ctx));

  // FIXME: Original codebase accounts for different MTU sizes. Really need to do that here, but
  // still working through my understanding of their windowing scheme.  In my testing with a modern
  // iPhone, I can just assume the ideal path for now
  client->tx_ctx.tx_window_size = PPOGATT_V0_WINDOW_SIZE;

  // The closest I've come to doing something right...
  const uint8_t desired_rx_window = MIN(PPOGATT_V1_DESIRED_RX_WINDOW_SIZE, PPOGATT_SN_MOD_DIV - 1);
  client->tx_ctx.rx_window_size = desired_rx_window;

  if (self_initiated) {
    client->tx_ctx.send_reset_packet_and_type = PPoGATTPacketTypeResetRequest;
    client->state = PPoGATTStateConnectedClosedAwaitingResetCompleteSelfInitiatedReset;
  } else {
    client->tx_ctx.send_reset_packet_and_type = PPoGATTPacketTypeResetComplete;
    client->state = PPoGATTStateConnectedClosedAwaitingResetCompleteRemoteInitiatedReset;
  }

  prv_send_next_packets(client);
}

/**
 * @brief Start reset process
 *
 * @param client Pointer to client instance
 */
static void prv_start_reset(PPoGATT_Client_t *client) {
  // TODO: A whole lotta checks...

  prv_enter_awaiting_reset_complete(client, true);
}

/*****************************************************************************
 * Characteristic Bindings
 *****************************************************************************/

/**
 * @brief Callback called on data characteristic notify
 *
 * @param conn Pointer to connection
 * @param params Params of characteristic
 * @param buf Incoming buffer
 * @param len Length of incoming data
 * @return Typically returns BT_GATT_ITER_STOP
 */
static uint8_t prv_on_data_notify_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                                     const void *buf, uint16_t len) {
  LOG_HEXDUMP_INF(buf, len, "Received notification:");

  return BT_GATT_ITER_STOP;
}

/**
 * @brief Callback when subscription completes
 *
 * @param conn Connection
 * @param err Error code (0 = success)
 * @param params Subscribe params
 */
static void prv_subscribe_cb(struct bt_conn *conn, uint8_t err,
                             struct bt_gatt_subscribe_params *params) {
  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, subscribe_params);

  if (err) {
    LOG_ERR("Subscribe failed: %u", err);
    prv_destroy_client(client);
    return;
  }

  LOG_INF("Subscribed to data characteristic");

  prv_start_reset(client);
}

/**
 * @brief Subscribe to data characteristic
 *
 * @param client Pointer to client instance
 */
static void prv_subscribe_to_data_char(PPoGATT_Client_t *client) {
  memset(&client->subscribe_params, 0, sizeof(client->subscribe_params));
  memset(&client->ccc_disc_params, 0, sizeof(client->ccc_disc_params));

  client->subscribe_params.notify = prv_on_data_notify_cb;
  client->subscribe_params.subscribe = prv_subscribe_cb;
  client->subscribe_params.value_handle = client->data_handle;
  client->subscribe_params.end_handle = client->service_end_handle;
  client->subscribe_params.disc_params = &client->ccc_disc_params;
  client->subscribe_params.ccc_handle = 0; /* Auto-discover CCCD */
  client->subscribe_params.value = BT_GATT_CCC_NOTIFY;

  int ret = bt_gatt_subscribe(prv_inst.conn, &client->subscribe_params);

  if (ret < 0) {
    LOG_ERR("Failed to subscribe to data characteristic: %d", ret);
    prv_destroy_client(client);
    return;
  }

  /* client->state = PPoGATT_CLIENT_STATE_DISCONNECTED_SUBSCRIBING_DATA; */
}

/**
 * @brief Callback for meta characteristic read
 *
 * @param conn Pointer to connection
 * @param err Error code during read
 * @param params Pointer to original parameters
 * @param data Pointer to incoming data
 * @param len Length of incoming data
 * @return Returns BT_GATT_ITER_STOP pretty much all the time... I think...
 */
static uint8_t prv_on_meta_read(struct bt_conn *conn, uint8_t err,
                                struct bt_gatt_read_params *params, const void *data,
                                uint16_t len) {
  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, read_params);

  if (err != 0) {
    LOG_ERR("Failed to read meta data: %u", err);
    prv_destroy_client(client);
    return BT_GATT_ITER_STOP;
  }

  PPoGATTMetaV1_t *meta = (PPoGATTMetaV1_t *)data;

  if (len < sizeof(PPoGATTMetaV0_t)) {
    LOG_ERR("Invalid meta size.");
    prv_destroy_client(client);
    return BT_GATT_ITER_STOP;
  }

  uint8_t meta_version = 0;

  client->meta.ppogatt_min_version = meta->ppogatt_min_version;
  client->meta.ppogatt_max_version = meta->ppogatt_max_version;
  client->meta.app_uuid = meta->app_uuid;

  if (len >= sizeof(PPoGATTMetaV1_t)) {
    meta_version++;
    if (meta->pp_session_type >= PPoGATTSessionTypeCount) {
      LOG_ERR("Invalid session type.");
      prv_destroy_client(client);
      return BT_GATT_ITER_STOP;
    }

    client->meta.pp_session_type = meta->pp_session_type;
  }

  char uuid_str[UUID_STR_LEN] = {0};
  uuid_to_string(&meta->app_uuid, uuid_str);

  LOG_INF(
      "Fetch Meta:\r\n"
      "\tMeta Version: %u\r\n"
      "\tMin Version: %u\r\n"
      "\tMax Version: %u\r\n"
      "\tUUID: %s\r\n"
      "\tSession Type: %u",
      meta_version, client->meta.ppogatt_min_version, client->meta.ppogatt_max_version, uuid_str,
      client->meta.pp_session_type);

  prv_subscribe_to_data_char(client);

  return BT_GATT_ITER_STOP;
}

/**
 * @brief Fetch meta from server
 *
 * @param client Pointer to client instance
 * @return Returns 0 on success
 */
static int prv_fetch_meta(PPoGATT_Client_t *client) {
  client->read_params.handle_count = 1;
  client->read_params.single.offset = 0;
  client->read_params.single.handle = client->meta_handle;
  client->read_params.func = prv_on_meta_read;

  int ret = bt_gatt_read(prv_inst.conn, &client->read_params);

  if (ret < 0) {
    LOG_ERR("Failed to queue meta read: %d", ret);
  }

  return ret;
}

/*****************************************************************************
 * BLE Bindings
 *****************************************************************************/

/**
 * @brief Callback when PPoGATT service is discovered
 *
 * @param handles Array of characteristic handles
 * @param service_end_handle End handle of the service (for CCCD discovery)
 */
static void prv_on_service_discovered(uint16_t *handles, uint16_t service_end_handle) {
  LOG_INF("PPoGATT service discovered!");

  PPoGATT_Client_t *client = prv_create_client();

  if (client == NULL) {
    // FIXME: Fatal?
    LOG_ERR("Unable to allocate new PPoGATT client.");

    return;
  }

  client->meta_handle = handles[PPOGATT_CHAR_META];
  client->data_handle = handles[PPOGATT_CHAR_DATA];
  client->service_end_handle = service_end_handle;

  int ret = prv_fetch_meta(client);
  if (ret != 0) {
    LOG_ERR("Failed to fetch client meta data: %d", ret);

    prv_destroy_client(client);
    return;
  }

  // FIXME: Guard with mutex. Original code had overarching BT lock.  Unsure if that is best
  // decision...
  sys_slist_append(&prv_inst.clients, (sys_snode_t *)client);
}

/**
 * @brief Called when service is removed from server
 */
static void prv_on_service_removed(void) {
  LOG_INF("PPoGATT service removed.");
}

/**
 * @brief Called if handles to service should be invalidated
 */
static void prv_on_invalidate_all(void) {
  LOG_INF("PPoGATT service invalidated.");
}

/**
 * @brief Called when connection is established
 *
 * @param conn Pointer to connection
 * @param err Error code during connection
 */
static void prv_on_connected(struct bt_conn *conn, uint8_t err) {
  if (err) {
    LOG_ERR("Connection failed: %d", err);
    return;
  }

  prv_inst.conn = bt_conn_ref(conn);
}

/**
 * @brief Called when BLE connection is broken
 *
 * @param conn Pointer to connection instance
 * @param err Reason for disconnection
 */
static void prv_on_disconnected(struct bt_conn *conn, uint8_t err) {
  prv_destroy_all_clients();

  bt_conn_unref(prv_inst.conn);
  prv_inst.conn = NULL;
}

/**
 * @brief BLE GATT Client Instance
 */
static ble_client_t prv_ble_client = {
    .client_name = "PPoGATT",
    .service_uuid = PEBBLE_BT_PPOGATT_SERVICE_UUID,
    .char_uuids = prv_ppogatt_char_uuids,
    .num_chars = PPOGATT_CHAR_COUNT,

    .on_service_discovered = prv_on_service_discovered,
    .on_service_removed = prv_on_service_removed,
    .on_invalidate_all = prv_on_invalidate_all,
};

BT_CONN_CB_DEFINE(prv_bt_callbacks) = {
    .connected = prv_on_connected,
    .disconnected = prv_on_disconnected,
};

/*****************************************************************************
 * Functions
 *****************************************************************************/

int ppogatt_client_init(void) {
  ble_conn_mgr_register_client(&prv_ble_client);
  return 0;
}
