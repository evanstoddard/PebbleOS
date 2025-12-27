/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_ppogatt_service.c
 * @author Evan Stoddard
 * @brief PPoGATT GATT Client implementation
 */

#include "ble_ppogatt_service.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ble/ble_conn_mgr.h"

#include "ppogatt_client.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_ppogatt_service);

/* Characteristic indices */
enum ppogatt_char_idx {
  PPOGATT_CHAR_DATA,
  PPOGATT_CHAR_META,
  PPOGATT_CHAR_COUNT,
};

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

static struct bt_uuid_128 prv_ppogatt_data_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_DATA_VAL);
static struct bt_uuid_128 prv_ppogatt_meta_uuid = BT_UUID_INIT_128(BT_UUID_PPOGATT_META_VAL);

static const struct bt_uuid *prv_ppogatt_char_uuids[PPOGATT_CHAR_COUNT] = {
    [PPOGATT_CHAR_DATA] = (const struct bt_uuid *)&prv_ppogatt_data_uuid,
    [PPOGATT_CHAR_META] = (const struct bt_uuid *)&prv_ppogatt_meta_uuid,
};

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

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
                             const void *buf, uint16_t len) {
  if (buf == NULL) {
    LOG_INF("Unsubscribed from notifications.");
    return BT_GATT_ITER_STOP;
  }

  return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief [TODO:description]
 *
 * @param handles [TODO:parameter]
 */
static void prv_on_service_discovered(uint16_t *handles) {}

/**
 * @brief [TODO:description]
 */
static void prv_on_service_removed(void) {
  ppogatt_client_manager_destroy_all();
}

/**
 * @brief [TODO:description]
 */
static void prv_on_invalidate_all(void) {
  ppogatt_client_manager_destroy_all();
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

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

int ble_ppogatt_service_init(void) {
  ble_conn_mgr_register_client(&prv_ble_client);

  ppogatt_client_manager_init();

  return 0;
}
