/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_conn_mgr.c
 * @author Evan Stoddard
 * @brief
 */

#include "ble_conn_mgr.h"

#include <zephyr/logging/log.h>

#include <zephyr/sys/slist.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include <zephyr/settings/settings.h>

#include "ble_advertising.h"
#include "services/pebble_pairing_service/pebble_pairing_service.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_conn_mgr);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Module instance state
 */
static struct {
  struct bt_conn *conn;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Callback invoked when connection security level changes.
 *        Kicks off GATT service discovery once security is established.
 *
 * @param conn Connection object
 * @param level New security level
 * @param err Security error (0 if successful)
 */
static void prv_on_security_changed(struct bt_conn *conn, bt_security_t level,
                                    enum bt_security_err err) {
  if (err != BT_SECURITY_ERR_SUCCESS) {
    LOG_ERR("Security change failed: %d", err);
    return;
  }

  LOG_INF("Security changed: level %d", level);
}

/*****************************************************************************
 * Connection Callbacks
 *****************************************************************************/

BT_CONN_CB_DEFINE(prv_conn_callbacks) = {
    .security_changed = prv_on_security_changed,
};

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

int ble_conn_mgr_init(void) {
  prv_inst.conn = NULL;

  ble_advertising_init();

  int ret = bt_enable(NULL);
  if (ret < 0) {
    LOG_ERR("Failed to enable BLE: %d", ret);
    return ret;
  }

  // TODO: This should be moved once zephyr settings fully integrated with
  // Pebble OS
  settings_load();

  pebble_pairing_service_init();

  return 0;
}
