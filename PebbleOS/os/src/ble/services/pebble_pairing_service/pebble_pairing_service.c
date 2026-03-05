/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_pairing_service.c
 * @author Evan Stoddard
 * @brief Device hosted BLE service to coordinate device pairing
 */

#include "pebble_pairing_service.h"

#include <errno.h>

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ble/pebble_bt.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pebble_pairing_service);

//! Connectivity Status characteristic (UUID 1 on the Pebble Base UUID)
#define PEBBLE_BT_PAIRING_SERVICE_CONNECTIVITY_STATUS_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(0x00000001))

//! Trigger Pairing characteristic (UUID 2 on the Pebble Base UUID)
//! Note: UUID 4 was used by the 3.14-rc Android App for V0 of the Connection
//! Param characteristic but never shipped externally.
#define PEBBLE_BT_PAIRING_SERVICE_TRIGGER_PAIRING_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(0x00000002))

//! Connection Parameters characteristic (UUID 5 on the Pebble Base UUID)
#define PEBBLE_BT_PAIRING_SERVICE_CONNECTION_PARAMETERS_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(0x00000005))

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * BLE Binding Declarations
 *****************************************************************************/

/**
 * @brief Read handler for the Connectivity Status characteristic
 *
 * @param conn Connection object
 * @param attr GATT attribute
 * @param buf Buffer to write response into
 * @param len Length of buffer
 * @param offset Read offset
 * @return Number of bytes read, or negative error code
 */
static ssize_t prv_on_read_conn_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset);

/**
 * @brief Read handler for the Trigger Pairing characteristic
 *
 * @param conn Connection object
 * @param attr GATT attribute
 * @param buf Buffer to write response into
 * @param len Length of buffer
 * @param offset Read offset
 * @return Number of bytes read, or negative error code
 */
static ssize_t prv_on_read_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                           void *buf, uint16_t len, uint16_t offset);

/**
 * @brief Write handler for the Trigger Pairing characteristic
 *
 * @param conn Connection object
 * @param attr GATT attribute
 * @param buf Buffer containing written data
 * @param len Length of data
 * @param offset Write offset
 * @param flags Write flags
 * @return Number of bytes written, or negative error code
 */
static ssize_t prv_on_write_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                            const void *buf, uint16_t len, uint16_t offset,
                                            uint8_t flags);

/**
 * @brief Read handler for the Connection Parameters characteristic
 *
 * @param conn Connection object
 * @param attr GATT attribute
 * @param buf Buffer to write response into
 * @param len Length of buffer
 * @param offset Read offset
 * @return Number of bytes read, or negative error code
 */
static ssize_t prv_on_read_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset);

/**
 * @brief Write handler for the Connection Parameters characteristic
 *
 * @param conn Connection object
 * @param attr GATT attribute
 * @param buf Buffer containing written data
 * @param len Length of data
 * @param offset Write offset
 * @param flags Write flags
 * @return Number of bytes written, or negative error code
 */
static ssize_t prv_on_write_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, uint16_t len, uint16_t offset,
                                        uint8_t flags);

/**
 * @brief CCC changed handler for the Connectivity Status characteristic
 *
 * @param attr GATT attribute
 * @param value New CCC value
 */
static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value);

/**
 * @brief CCC changed handler for the Connection Parameters characteristic
 *
 * @param attr GATT attribute
 * @param value New CCC value
 */
static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value);

BT_GATT_SERVICE_DEFINE(
    prv_pebble_pairing_svc, BT_GATT_PRIMARY_SERVICE(PEBBLE_BT_PAIRING_SERVICE_UUID),

    /* Connection Status characteristic */
    BT_GATT_CHARACTERISTIC(PEBBLE_BT_PAIRING_SERVICE_CONNECTIVITY_STATUS_UUID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
                           prv_on_read_conn_status, NULL, NULL),
    BT_GATT_CCC(prv_on_conn_status_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Trigger Pairing characteristic */
    BT_GATT_CHARACTERISTIC(PEBBLE_BT_PAIRING_SERVICE_TRIGGER_PAIRING_UUID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, prv_on_read_trigger_pairing,
                           prv_on_write_trigger_pairing, NULL),

    /* Connection Parameters characteristic */
    BT_GATT_CHARACTERISTIC(PEBBLE_BT_PAIRING_SERVICE_CONNECTION_PARAMETERS_UUID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, prv_on_read_conn_params,
                           prv_on_write_conn_params, NULL),
    BT_GATT_CCC(prv_on_conn_params_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/*****************************************************************************
 * BLE Binding Defintins
 *****************************************************************************/

static ssize_t prv_on_read_conn_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset) {
  LOG_INF("Phone requesting connection status.");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static ssize_t prv_on_read_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                           void *buf, uint16_t len, uint16_t offset) {
  LOG_INF("Phone requestion trigger pairing data?");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static ssize_t prv_on_write_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                            const void *buf, uint16_t len, uint16_t offset,
                                            uint8_t flags) {
  LOG_INF("Phone writing trigger pairing data?");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static ssize_t prv_on_read_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset) {
  LOG_INF("Phone requesting connection params?");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static ssize_t prv_on_write_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, uint16_t len, uint16_t offset,
                                        uint8_t flags) {
  LOG_INF("Phone writing connection params?");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value) {
  LOG_INF("Connection status characteristic config changed.");
}

static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value) {
  LOG_INF("Connection params characteristic config changed.");
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int pebble_pairing_service_init(void) {
  return -ENOTSUP;
}
