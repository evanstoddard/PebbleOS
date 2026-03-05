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

#include "ble/services/pebble_pairing_service/pebble_pairing_service_private.h"

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

/**
 * @brief Private instance data
 */
static struct {
  const struct bt_gatt_attr *conn_status_attr;
  const struct bt_gatt_attr *conn_params_attr;
} prv_inst;

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

/**
 * @brief Handle reception of remote param mgmt setings
 *
 * @param conn Pointer to connection
 * @param settings Pointer to incoming settings
 * @return Returns a BT_ATT_ERR value (BT_ATT_ERR_SUCCESS if no error)
 */
static uint8_t prv_handle_remote_param_settings(struct bt_conn *conn,
                                                PebblePairingServiceConnParamsWrite_t *settings,
                                                uint16_t buf_len) {
  size_t num_param_sets = (buf_len - sizeof(PebblePairingServiceConnParamsWrite_t)) /
                          sizeof(PebblePairingServiceConnParamSet_t);

  LOG_INF(
      "Received remote param settings mgmt cmd:\r\n"
      "\tRemote managing: %u\r\n"
      "\tNum param sets: %zu\r\n",
      settings->remote_param_mgmt_settings.is_remote_device_managing_connection_parameters,
      num_param_sets);

  // TODO: Actually handle these settings...

  return BT_ATT_ERR_SUCCESS;
}

/**
 * @brief Populate connectivity status struct for given connection
 *
 * @param conn Pointer to connection
 * @param status Pointer to struct to write info into
 */
static void prv_populate_conn_status_struct(struct bt_conn *conn,
                                            PebblePairingServiceConnectivityStatus_t *status) {
  if (conn == NULL || status == NULL) {
    return;
  }

  struct bt_conn_info conn_info = {0};
  bt_conn_get_info(conn, &conn_info);

  memset(status, 0, sizeof(PebblePairingServiceConnectivityStatus_t));

  status->ble_is_connected = true;
  status->ble_is_bonded = bt_le_bond_exists(conn_info.id, conn_info.le.dst);
  status->ble_is_encrypted = (conn_info.security.level >= BT_SECURITY_L2);
  status->supports_pinning_without_security_request = true;
  status->is_reversed_ppogatt_enabled = true;

  LOG_INF(
      "Fetched connection status: \r\n"
      "\tConnected: %u\r\n"
      "\tBonded: %u\r\n"
      "\tEncrpyted: %u\r\n",
      status->ble_is_connected, status->ble_is_bonded, status->ble_is_encrypted);
}

/**
 * @brief Notify phone of changes to connection status
 *
 * @param conn Pointer to connection
 */
static void prv_notify_conn_status(struct bt_conn *conn) {
  PebblePairingServiceConnectivityStatus_t status = {0};
  prv_populate_conn_status_struct(conn, &status);

  if (prv_inst.conn_status_attr == NULL) {
    return;
  }

  int ret = bt_gatt_notify(conn, prv_inst.conn_status_attr, &status, sizeof(status));

  if (ret < 0) {
    LOG_ERR("Failed to notify connection status update: %d", ret);
  }
}

/**
 * @brief Handle a pairing trigger request and log its contents
 *
 * @param conn Pointer to connection
 * @param buf Pointer to written data
 * @param len Length of written data
 * @return Returns a BT_ATT_ERR value (BT_ATT_ERR_SUCCESS if no error)
 */
static uint8_t prv_handle_pairing_trigger_request(struct bt_conn *conn, const void *buf,
                                                  uint16_t len) {
  const PairingTriggerRequestData_t *req = (const PairingTriggerRequestData_t *)buf;

  LOG_INF(
      "Pairing trigger request:\r\n"
      "\tshould_pin_address: %u\r\n"
      "\tno_slave_security_request: %u\r\n"
      "\tshould_force_slave_security_request: %u\r\n"
      "\tshould_auto_accept_re_pairing: %u\r\n"
      "\tis_reversed_ppogatt_enabled: %u",
      req->should_pin_address, req->no_slave_security_request,
      req->should_force_slave_security_request, req->should_auto_accept_re_pairing,
      req->is_reversed_ppogatt_enabled);

  struct bt_conn_info conn_info = {0};
  bt_conn_get_info(conn, &conn_info);

  // Set security level to require encryption if needed
  if ((!req->no_slave_security_request && conn_info.security.level < BT_SECURITY_L2) ||
      req->should_force_slave_security_request) {
    int ret = bt_conn_set_security(conn, BT_SECURITY_L2);

    if (ret < 0) {
      LOG_ERR("Failed to update security level: %d", ret);
    }

    prv_notify_conn_status(conn);
  }

  return BT_ATT_ERR_SUCCESS;
}

/*****************************************************************************
 * BLE Binding Defintins
 *****************************************************************************/

static ssize_t prv_on_read_conn_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset) {
  PebblePairingServiceConnectivityStatus_t status = {0};

  prv_populate_conn_status_struct(conn, &status);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &status, sizeof(status));
}

static ssize_t prv_on_read_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                           void *buf, uint16_t len, uint16_t offset) {
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
  if ((!no_sec_req && conn_info.security.level < BT_SECURITY_L2) || force_sec_req) {
    int ret = bt_conn_set_security(conn, BT_SECURITY_L2);

    if (ret < 0) {
      LOG_ERR("Failed to update security level: %d", ret);
    }

    prv_notify_conn_status(conn);
  }

  return len;
}

static ssize_t prv_on_write_trigger_pairing(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                            const void *buf, uint16_t len, uint16_t offset,
                                            uint8_t flags) {
  uint8_t ret = prv_handle_pairing_trigger_request(conn, buf, len);
  if (ret != BT_ATT_ERR_SUCCESS) {
    return BT_GATT_ERR(ret);
  }

  return len;
}

static ssize_t prv_on_read_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                       void *buf, uint16_t len, uint16_t offset) {
  LOG_INF("Phone requesting connection params?");

  return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
}

static ssize_t prv_on_write_conn_params(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, uint16_t len, uint16_t offset,
                                        uint8_t flags) {
  PebblePairingServiceConnParamsWrite_t *conn_params_req =
      (PebblePairingServiceConnParamsWrite_t *)buf;

  uint8_t ret = BT_ATT_ERR_SUCCESS;

  switch (conn_params_req->cmd) {
    case PebblePairingServiceConnParamsWriteCmd_SetRemoteParamMgmtSettings:
      ret = prv_handle_remote_param_settings(conn, conn_params_req, len);
      break;
    default:
      LOG_INF("Unhandled write conn params cmd: 0x%02X", conn_params_req->cmd);
      ret = BT_ATT_ERR_NOT_SUPPORTED;
      break;
  }

  return BT_GATT_ERR(ret);
}

static void prv_on_conn_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value) {
  if (value & BT_GATT_CCC_NOTIFY) {
    prv_inst.conn_status_attr = attr - 1;
  } else {
    prv_inst.conn_status_attr = NULL;
  }
}

static void prv_on_conn_params_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value) {
  if (value & BT_GATT_CCC_NOTIFY) {
    prv_inst.conn_params_attr = attr - 1;
  } else {
    prv_inst.conn_params_attr = NULL;
  }
}

/**
 * @brief Callback from BLE stack, indicating a disconnection
 *
 * @param conn Pointer to connection
 * @param reason Disconnect reason
 */
static void prv_on_disconnected(struct bt_conn *conn, uint8_t reason) {
  prv_inst.conn_status_attr = NULL;
  prv_inst.conn_params_attr = NULL;
}

/**
 * @brief Callback from BLE stack, indicating security level changed
 *
 * @param conn Pointer to connection
 * @param level Level changed to
 * @param err Any error indication
 */
static void prv_on_security_changed(struct bt_conn *conn, bt_security_t level,
                                    enum bt_security_err err) {
  if (err) {
    LOG_ERR("Security change failed: level %d, err %d", level, err);
  } else {
    LOG_INF("Security changed: level %d", level);
    prv_notify_conn_status(conn);
  }
}

BT_CONN_CB_DEFINE(prv_conn_callbacks) = {
    .disconnected = prv_on_disconnected,
    .security_changed = prv_on_security_changed,
};
/*****************************************************************************
 * Functions
 *****************************************************************************/

int pebble_pairing_service_init(void) {
  return 0;
}
