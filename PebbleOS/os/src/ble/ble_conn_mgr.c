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

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include "ble_advertising.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_conn_mgr);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static struct {
  struct bt_conn *conn;
  uint16_t conn_handle;
} prv_inst;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int ble_conn_mgr_init(void) {

  prv_inst.conn = NULL;

  ble_advertising_init();

  int ret = bt_enable(NULL);
  if (ret < 0) {
    LOG_ERR("Failed to enable BLE: %d", ret);
    return ret;
  }

  return 0;
}
