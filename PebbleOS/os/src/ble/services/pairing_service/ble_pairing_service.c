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
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ble/ble_uuid.h"
#include "zephyr/bluetooth/conn.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_pairing_service);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static struct {
  bool initialized;
} prv_inst;

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
  struct bt_conn_info conn_info = {0};
  bt_conn_get_info(conn, &conn_info);

  PPSConnectivityStatus_t status = {0};

  status.ble_is_connected = true;

  status.ble_is_bonded = bt_le_bond_exists(conn_info.id, conn_info.le.dst);

  status.ble_is_encrypted = (conn_info.security.level > BT_SECURITY_L2);

  LOG_INF("Writing connection status: \r\n"
          "\tConnected: %u\r\n"
          "\tBonded: %u\r\n"
          "\tEncrpyted: %u\r\n",
          status.ble_is_connected, status.ble_is_bonded,
          status.ble_is_encrypted);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &status,
                           sizeof(status));
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
  if (len < 1) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  LOG_HEXDUMP_INF(buf, len, "Trigger Pairing Payload:");

  uint8_t pairing_flags = ((uint8_t *)buf)[0];

  struct bt_conn_info conn_info = {0};
  bt_conn_get_info(conn, &conn_info);

  bool no_sec_req = (pairing_flags & 0x02) != 0;
  bool force_sec_req = (pairing_flags & 0x04) != 0;

  // Set security level to require encryption if needed
  if ((!no_sec_req && conn_info.security.level < BT_SECURITY_L2) ||
      force_sec_req) {
    bt_conn_set_security(conn, BT_SECURITY_L2);
  }

  return len;
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
  LOG_HEXDUMP_INF(buf, len, "Write Conn Params:");

  // TODO: Set params
  return len;
}

static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value) {}

static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr,
                                           uint16_t value) {}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param bonded [TODO:parameter]
 */
static void prv_on_pairing_complete(struct bt_conn *conn, bool bonded) {
  LOG_INF("Pairing complete!  Bonded: %s", bonded ? "true" : "false");
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int ble_pairing_service_init(void) {
  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  static struct bt_conn_auth_info_cb auth_cb = {.pairing_complete =
                                                    prv_on_pairing_complete};

  bt_conn_auth_info_cb_register(&auth_cb);

  prv_inst.initialized = true;

  return 0;
}
