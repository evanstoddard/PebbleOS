/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_pairing_service.c
 * @author Evan Stoddard
 * @brief
 */

#include "ble_pairing_service.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ble/ble_uuid.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_pairing_service);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * BLE Service Bindings
 *****************************************************************************/

#define BT_UUID_PEBBLE_PAIRING_SERVICE BT_UUID_DECLARE_16(0xFED9)

/* Connection Status: 00000001-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPS_CONN_STATUS                                                \
  BT_UUID_DECLARE_128(PEBBLE_BASE_UUID_BYTES, 0x01, 0x00, 0x00, 0x00)

/* Trigger Pairing: 00000002-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPS_TRIGGER_PAIRING                                            \
  BT_UUID_DECLARE_128(PEBBLE_BASE_UUID_BYTES, 0x02, 0x00, 0x00, 0x00)

/* Connection Parameters: 00000005-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPS_CONN_PARAMS                                                \
  BT_UUID_DECLARE_128(PEBBLE_BASE_UUID_BYTES, 0x05, 0x00, 0x00, 0x00)

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @param offset [TODO:parameter]
 * @return [TODO:return]
 */
static ssize_t prv_on_read_conn_status(struct bt_conn *conn,
                                       const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len,
                                       uint16_t offset);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @param offset [TODO:parameter]
 * @return [TODO:return]
 */
static ssize_t prv_on_read_trigger_pairing(struct bt_conn *conn,
                                           const struct bt_gatt_attr *attr,
                                           void *buf, uint16_t len,
                                           uint16_t offset);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @param offset [TODO:parameter]
 * @param flags [TODO:parameter]
 * @return [TODO:return]
 */
static ssize_t prv_on_write_trigger_pairing(struct bt_conn *conn,
                                            const struct bt_gatt_attr *attr,
                                            const void *buf, uint16_t len,
                                            uint16_t offset, uint8_t flags);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @param offset [TODO:parameter]
 * @return [TODO:return]
 */
static ssize_t prv_on_read_conn_params(struct bt_conn *conn,
                                       const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len,
                                       uint16_t offset);

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @param offset [TODO:parameter]
 * @param flags [TODO:parameter]
 * @return [TODO:return]
 */
static ssize_t prv_on_write_conn_params(struct bt_conn *conn,
                                        const struct bt_gatt_attr *attr,
                                        const void *buf, uint16_t len,
                                        uint16_t offset, uint8_t flags);

/**
 * @brief [TODO:description]
 *
 * @param attr [TODO:parameter]
 * @param value [TODO:parameter]
 */
static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value);

/**
 * @brief [TODO:description]
 *
 * @param attr [TODO:parameter]
 * @param value [TODO:parameter]
 */
static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value);

BT_GATT_SERVICE_DEFINE(
    prv_pebble_pairing_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_PEBBLE_PAIRING_SERVICE),

    /* Connection Status characteristic */
    BT_GATT_CHARACTERISTIC(BT_UUID_PPS_CONN_STATUS,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ, prv_on_read_conn_status, NULL,
                           NULL),
    BT_GATT_CCC(prv_on_conn_status_ccc_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Trigger Pairing characteristic */
    BT_GATT_CHARACTERISTIC(BT_UUID_PPS_TRIGGER_PAIRING,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           prv_on_read_trigger_pairing,
                           prv_on_write_trigger_pairing, NULL),

    /* Connection Parameters characteristic */
    BT_GATT_CHARACTERISTIC(BT_UUID_PPS_CONN_PARAMS,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE |
                               BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           prv_on_read_conn_params, prv_on_write_conn_params,
                           NULL),
    BT_GATT_CCC(prv_on_conn_params_ccc_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

static ssize_t prv_on_read_conn_status(struct bt_conn *conn,
                                       const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len,
                                       uint16_t offset) {
  return -ENOTSUP;
}

static ssize_t prv_on_read_trigger_pairing(struct bt_conn *conn,
                                           const struct bt_gatt_attr *attr,
                                           void *buf, uint16_t len,
                                           uint16_t offset) {
  return -ENOTSUP;
}

static ssize_t prv_on_write_trigger_pairing(struct bt_conn *conn,
                                            const struct bt_gatt_attr *attr,
                                            const void *buf, uint16_t len,
                                            uint16_t offset, uint8_t flags) {
  return -ENOTSUP;
}

static ssize_t prv_on_read_conn_params(struct bt_conn *conn,
                                       const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len,
                                       uint16_t offset) {
  return -ENOTSUP;
}

static ssize_t prv_on_write_conn_params(struct bt_conn *conn,
                                        const struct bt_gatt_attr *attr,
                                        const void *buf, uint16_t len,
                                        uint16_t offset, uint8_t flags) {
  return -ENOTSUP;
}

static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value) {}

static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value) {}

/*****************************************************************************
 * Functions
 *****************************************************************************/
