/*
 * Copyright (C) Ovyl
 */

/**
 * @file ppogatt_client.c
 * @author Evan Stoddard
 * @brief
 */

#include "ppogatt_client.h"

#include <stddef.h>

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include <zephyr/sys/slist.h>

#include "ppogatt_client_internal.h"

#include "ble/ble_conn_mgr.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ppogatt_client);

/* PPoGATT Service UUID: 10000000-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_SVC_VAL \
  BT_UUID_128_ENCODE(0x10000000, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)
#define BT_UUID_PPOGATT_SVC BT_UUID_DECLARE_128(BT_UUID_PPOGATT_SVC_VAL)

/* PPoGATT Data Characteristic UUID: 10000001-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_DATA_VAL \
  BT_UUID_128_ENCODE(0x10000001, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)

/* PPoGATT Meta Characteristic UUID: 10000002-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_META_VAL \
  BT_UUID_128_ENCODE(0x10000002, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum {
  PPOGATT_CHAR_DATA,
  PPOGATT_CHAR_META,
  PPOGATT_CHAR_COUNT,
} PPoGATT_Char_Idx_t;

/**
 * @brief [TODO:description]
 */
static struct bt_uuid_128 prv_ppogatt_data_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_DATA_VAL);

/**
 * @brief [TODO:description]
 */
static struct bt_uuid_128 prv_ppogatt_meta_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_META_VAL);

/**
 * @brief [TODO:description]
 */
static const struct bt_uuid *prv_ppogatt_char_uuids[PPOGATT_CHAR_COUNT] = {
    [PPOGATT_CHAR_DATA] = (const struct bt_uuid *)&prv_ppogatt_data_uuid,
    [PPOGATT_CHAR_META] = (const struct bt_uuid *)&prv_ppogatt_meta_uuid,
};

/**
 * @brief Private instance
 */
static struct {
  sys_slist_t clients;

  struct bt_conn *conn;
} prv_inst;

/*****************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static void prv_destroy_client(PPoGATT_Client_t *client);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param params [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_notify_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                             const void *buf, uint16_t len);

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

static void prv_write_reset_packet(PPoGATT_Client_t *client) {
  PPoGATT_Reset_Packet_t packet;
  packet.header.type = PPoGATT_PACKET_TYPE_RESET;
  packet.header.sn = 0;
  packet.version = 1;
  memcpy(packet.serial_num, "0123456789AB", 12);

  int ret = bt_gatt_write_without_response(prv_inst.conn, client->ble_handles.data_handle, &packet,
                                           sizeof(packet), false);

  if (ret < 0) {
    LOG_ERR("Failed to write reset packet: %d", ret);
    prv_destroy_client(client);
    return;
  }

  client->state = PPoGATT_CLIENT_STATE_CONNECTED_CLOSED_AWAIT_SELF_RESET_COMPLETE;
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

  prv_write_reset_packet(CONTAINER_OF(params, PPoGATT_Client_t, subscribe_params));
}

/**
 * @brief Subscribe to data characteristic notifications
 *
 * @param client PPoGATT client instance
 */
static void prv_client_subscribe(PPoGATT_Client_t *client) {
  memset(&client->subscribe_params, 0, sizeof(client->subscribe_params));
  memset(&client->ccc_disc_params, 0, sizeof(client->ccc_disc_params));

  client->subscribe_params.notify = prv_notify_cb;
  client->subscribe_params.subscribe = prv_subscribe_cb;
  client->subscribe_params.value_handle = client->ble_handles.data_handle;
  client->subscribe_params.end_handle = client->ble_handles.service_end_handle;
  client->subscribe_params.disc_params = &client->ccc_disc_params;
  client->subscribe_params.ccc_handle = 0; /* Auto-discover CCCD */
  client->subscribe_params.value = BT_GATT_CCC_NOTIFY;

  int ret = bt_gatt_subscribe(prv_inst.conn, &client->subscribe_params);

  if (ret < 0) {
    LOG_ERR("Failed to subscribe to data characteristic: %d", ret);
    prv_destroy_client(client);
    return;
  }

  client->state = PPoGATT_CLIENT_STATE_DISCONNECTED_SUBSCRIBING_DATA;
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 * @param params [TODO:parameter]
 * @param data [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_on_meta_read(struct bt_conn *conn, uint8_t err,
                                struct bt_gatt_read_params *params, const void *data,
                                uint16_t len) {
  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, read_params);

  if (err != 0) {
    LOG_ERR("Error reading meta: %u", err);
    return BT_GATT_ITER_STOP;
  }

  memcpy(&client->meta, data, len);

  LOG_INF(
      "Fetched PPoGATT Session Meta:\r\n"
      "\tMin Version: %u\r\n"
      "\tMax Version: %u\r\n",
      client->meta.min_version, client->meta.max_version);

  prv_client_subscribe(client);

  return BT_GATT_ITER_STOP;
}

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 * @return [TODO:return]
 */
static int prv_fetch_metadata(PPoGATT_Client_t *client) {
  client->read_params.handle_count = 1;
  client->read_params.single.offset = 0;
  client->read_params.single.handle = client->ble_handles.meta_handle;
  client->read_params.func = prv_on_meta_read;

  LOG_INF("Reading from handle: 0x%04X", client->ble_handles.meta_handle);

  int ret = bt_gatt_read(prv_inst.conn, &client->read_params);

  if (ret < 0) {
    LOG_ERR("Failed to queue meta read: %d", ret);
    prv_destroy_client(client);
  }

  return ret;
}

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
static PPoGATT_Client_t *prv_create_client(void) {
  // FIXME: Need to properly handle kernel vs process heap.
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)k_malloc(sizeof(PPoGATT_Client_t));

  if (client == NULL) {
    return NULL;
  }

  memset(client, 0, sizeof(PPoGATT_Client_t));

  sys_slist_prepend(&prv_inst.clients, (sys_snode_t *)client);

  return client;
}

/**
 * @brief [TODO:description]
 */
static void prv_destroy_client(PPoGATT_Client_t *client) {
  sys_slist_find_and_remove(&prv_inst.clients, (sys_snode_t *)client);

  k_free(client);
}

/**
 * @brief [TODO:description]
 */
static void prv_destroy_all_clients(void) {
  PPoGATT_Client_t *client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);

  while (client) {
    prv_destroy_client(client);

    client = (PPoGATT_Client_t *)sys_slist_peek_head(&prv_inst.clients);
  }
}

static uint8_t prv_notify_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                             const void *buf, uint16_t len) {
  if (buf == NULL) {
    LOG_INF("Unsubscribed from notifications.");
    return BT_GATT_ITER_STOP;
  }

  PPoGATT_Packet_Header_t *header = (PPoGATT_Packet_Header_t *)buf;

  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, subscribe_params);

  LOG_HEXDUMP_INF(buf, len, "PPoGATT Data Notification:");

  return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief Callback when PPoGATT service is discovered
 *
 * @param handles Array of characteristic handles
 * @param service_end_handle End handle of the service (for CCCD discovery)
 */
static void prv_on_service_discovered(uint16_t *handles, uint16_t service_end_handle) {
  PPoGATT_Client_t *client = prv_create_client();

  if (client == NULL) {
    LOG_ERR("Failed to create client.");
    return;
  }

  LOG_INF(
      "Service discovered:\r\n"
      "\tMeta Handle: 0x%04X\r\n"
      "\tData Handle: 0x%04X\r\n"
      "\tService End Handle: 0x%04X\r\n",
      handles[PPOGATT_CHAR_META], handles[PPOGATT_CHAR_DATA], service_end_handle);

  client->ble_handles.data_handle = handles[PPOGATT_CHAR_DATA];
  client->ble_handles.meta_handle = handles[PPOGATT_CHAR_META];
  client->ble_handles.service_end_handle = service_end_handle;

  prv_fetch_metadata(client);
}

/**
 * @brief [TODO:description]
 */
static void prv_on_service_removed(void) {
  prv_destroy_all_clients();
}

/**
 * @brief [TODO:description]
 */
static void prv_on_invalidate_all(void) {
  prv_destroy_all_clients();
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 */
static void prv_on_connected(struct bt_conn *conn, uint8_t err) {
  if (err) {
    LOG_ERR("Connection failed: %d", err);
    return;
  }

  prv_inst.conn = bt_conn_ref(conn);
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 */
static void prv_on_disconnected(struct bt_conn *conn, uint8_t err) {
  if (prv_inst.conn == NULL) {
    return;
  }

  bt_conn_unref(prv_inst.conn);
  prv_inst.conn = NULL;
}

/*****************************************************************************
 * BLE GATT Client Definition
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static ble_client_t prv_ble_client = {
    .client_name = "PPoGATT",
    .service_uuid = BT_UUID_PPOGATT_SVC,
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

void ppogatt_client_init(void) {
  sys_slist_init(&prv_inst.clients);

  ble_conn_mgr_register_client(&prv_ble_client);
}
