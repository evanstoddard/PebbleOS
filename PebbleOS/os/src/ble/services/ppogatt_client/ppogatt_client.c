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

#include "ble/ble_conn_mgr.h"
#include "ble/pebble_bt.h"

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
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

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
