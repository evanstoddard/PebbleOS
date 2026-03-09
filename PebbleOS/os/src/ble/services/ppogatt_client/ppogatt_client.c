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
 * Private Functions
 *****************************************************************************/

/**
 * @brief Instantiate and initialize new PPoGATT client
 *
 * @return Returns pointer to new client
 */
PPoGATT_Client_t *prv_create_client(void) {
  PPoGATT_Client_t *client = kernel_heap_malloc(sizeof(PPoGATT_Client_t));

  if (client == NULL) {
    return NULL;
  }

  memset(client, 0, sizeof(PPoGATT_Client_t));

  return client;
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
  if (err != 0) {
    LOG_ERR("Failed to read meta data: %u", err);
    // FIXME: Need a way to signal that this PPoGATT client needs to be destroyed
    return BT_GATT_ITER_STOP;
  }

  PPoGATT_Client_t *client = CONTAINER_OF(params, PPoGATT_Client_t, read_params);

  PPoGATTMetaV1_t *meta = (PPoGATTMetaV1_t *)data;

  if (len < sizeof(PPoGATTMetaV0_t)) {
    LOG_ERR("Invalid meta size.");
    // FIXME: Need a way to signal that this PPoGATT client needs to be destroyed
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
      // FIXME: Need a way to signal that this PPoGATT client needs to be destroyed
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

  return BT_GATT_ITER_STOP;
}

/**
 * @brief Fetch meta from server
 *
 * @param client Pointer to client instance
 * @return Returns 0 on success
 */
int prv_fetch_meta(PPoGATT_Client_t *client) {
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
    // TODO: Fatal?
    LOG_ERR("Unable to allocate new PPoGATT client.");

    return;
  }

  client->meta_handle = handles[PPOGATT_CHAR_META];
  client->data_handle = handles[PPOGATT_CHAR_DATA];
  client->service_end_handle = service_end_handle;

  int ret = prv_fetch_meta(client);
  if (ret != 0) {
    LOG_ERR("Failed to fetch client meta data: %d", ret);

    // FIXME: Deallocate client
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
  if (prv_inst.conn == NULL) {
    return;
  }

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
