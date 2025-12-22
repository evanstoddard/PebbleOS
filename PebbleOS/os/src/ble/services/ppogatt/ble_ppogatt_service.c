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

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_ppogatt_service);

#define BT_UUID_PPOGATT_SVC_VAL                                                \
  BT_UUID_128_ENCODE(0x10000000, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)
#define BT_UUID_PPOGATT_SVC BT_UUID_DECLARE_128(BT_UUID_PPOGATT_SVC_VAL)

/* PPoGATT Data Characteristic UUID: 10000001-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_DATA_VAL                                               \
  BT_UUID_128_ENCODE(0x10000001, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)
#define BT_UUID_PPOGATT_DATA BT_UUID_DECLARE_128(BT_UUID_PPOGATT_DATA_VAL)

/* PPoGATT Meta Characteristic UUID: 10000002-328E-0FBB-C642-1AA6699BDADA */
#define BT_UUID_PPOGATT_META_VAL                                               \
  BT_UUID_128_ENCODE(0x10000002, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)
#define BT_UUID_PPOGATT_META BT_UUID_DECLARE_128(BT_UUID_PPOGATT_META_VAL)

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct {
  /* Client connection state - reset on each new connection */
  struct {
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t data_handle;
    uint16_t meta_handle;

    struct bt_gatt_discover_params discover_params;
    struct bt_gatt_discover_params ccc_discover_params;
    struct bt_gatt_subscribe_params subscribe_params;
  } client;
} prv_inst;

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
static uint8_t prv_notify_cb(struct bt_conn *conn,
                             struct bt_gatt_subscribe_params *params,
                             const void *buf, uint16_t len) {
  if (buf == NULL) {
    LOG_INF("Unsubscribed from notifications.");
    return BT_GATT_ITER_STOP;
  }

  /* TODO: Handle PPoGATT protocol here */

  return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param err [TODO:parameter]
 * @param params [TODO:parameter]
 */
static void prv_subscribe_cb(struct bt_conn *conn, uint8_t err,
                             struct bt_gatt_subscribe_params *params) {
  if (err) {
    LOG_ERR("Subscribe failed: %d", err);
    return;
  }

  LOG_INF("Subscribed to PPoGATT Data notifications (handle: 0x%04X)",
          params->value_handle);
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param params [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_discover_chars_cb(struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr,
                                     struct bt_gatt_discover_params *params) {
  if (attr == NULL) {
    /* Done discovering characteristics */
    if (prv_inst.client.data_handle == 0) {
      LOG_ERR("Data characteristic not found");
      return BT_GATT_ITER_STOP;
    }

    LOG_INF("Subscribing to Data characteristic (handle: 0x%04X)...",
            prv_inst.client.data_handle);

    /* Subscribe with auto CCC discovery */
    prv_inst.client.subscribe_params.notify = prv_notify_cb;
    prv_inst.client.subscribe_params.subscribe = prv_subscribe_cb;
    prv_inst.client.subscribe_params.value = BT_GATT_CCC_NOTIFY;
    prv_inst.client.subscribe_params.value_handle = prv_inst.client.data_handle;
    prv_inst.client.subscribe_params.ccc_handle = 0; /* Auto-discover */
    prv_inst.client.subscribe_params.disc_params =
        &prv_inst.client.ccc_discover_params;
    prv_inst.client.subscribe_params.end_handle =
        prv_inst.client.service_end_handle;

    int err = bt_gatt_subscribe(conn, &prv_inst.client.subscribe_params);
    if (err) {
      LOG_ERR("Subscribe failed: %d", err);
    }

    return BT_GATT_ITER_STOP;
  }

  struct bt_gatt_chrc *chrc = attr->user_data;

  if (bt_uuid_cmp(chrc->uuid, BT_UUID_PPOGATT_DATA) == 0) {
    prv_inst.client.data_handle = chrc->value_handle;
    LOG_INF("Found Data characteristic: handle 0x%04X",
            prv_inst.client.data_handle);
  } else if (bt_uuid_cmp(chrc->uuid, BT_UUID_PPOGATT_META) == 0) {
    prv_inst.client.meta_handle = chrc->value_handle;
    LOG_INF("Found Meta characteristic: handle 0x%04X",
            prv_inst.client.meta_handle);
  }

  return BT_GATT_ITER_CONTINUE;
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param attr [TODO:parameter]
 * @param params [TODO:parameter]
 * @return [TODO:return]
 */
static uint8_t prv_discover_service_cb(struct bt_conn *conn,
                                       const struct bt_gatt_attr *attr,
                                       struct bt_gatt_discover_params *params) {
  if (attr == NULL) {
    LOG_ERR("PPoGATT service not found");
    return BT_GATT_ITER_STOP;
  }

  struct bt_gatt_service_val *service = attr->user_data;

  prv_inst.client.service_start_handle = attr->handle;
  prv_inst.client.service_end_handle = service->end_handle;

  LOG_INF("Found PPoGATT service: handles 0x%04X - 0x%04X",
          prv_inst.client.service_start_handle,
          prv_inst.client.service_end_handle);

  /* Discover characteristics within this service */
  params->uuid = NULL; /* Discover all characteristics */
  params->start_handle = prv_inst.client.service_start_handle;
  params->end_handle = prv_inst.client.service_end_handle;
  params->type = BT_GATT_DISCOVER_CHARACTERISTIC;
  params->func = prv_discover_chars_cb;

  int err = bt_gatt_discover(conn, params);
  if (err) {
    LOG_ERR("Characteristic discovery failed: %d", err);
  }

  return BT_GATT_ITER_STOP;
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 */
static void prv_start_discovery(struct bt_conn *conn) {
  /* Reset client state only */
  memset(&prv_inst.client, 0, sizeof(prv_inst.client));

  prv_inst.client.discover_params.uuid = BT_UUID_PPOGATT_SVC;
  prv_inst.client.discover_params.func = prv_discover_service_cb;
  prv_inst.client.discover_params.start_handle = 0x0001;
  prv_inst.client.discover_params.end_handle = 0xFFFF;
  prv_inst.client.discover_params.type = BT_GATT_DISCOVER_PRIMARY;

  int err = bt_gatt_discover(conn, &prv_inst.client.discover_params);
  if (err) {
    LOG_ERR("Discovery failed to start: %d", err);
  }
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
  }
}

/**
 * @brief [TODO:description]
 *
 * @param conn [TODO:parameter]
 * @param level [TODO:parameter]
 * @param err [TODO:parameter]
 */
static void prv_on_security_changed(struct bt_conn *conn, bt_security_t level,
                                    enum bt_security_err err) {
  if (err != 0) {
    LOG_ERR("Security change failed: %d", err);
    return;
  }

  if (level < BT_SECURITY_L2) {
    return;
  }

  LOG_INF("Security established, starting PPoGATT discovery...");

  /*
   * NOTE: We kick of discovery here since the PPoGATT service needs to
   * connected to after pairing and after an connection is established with the
   * phone.  The zephyr discovery manager can only be started by one module at a
   * time, so we'll probably want to centralize this at a later time.  My
   * thought process is to have some parent module that clients can register
   * with so they can be run sequentially.  Those services can be denoted as
   * required, optional, etc, and define what happens if discovery fails.
   */
  prv_start_discovery(conn);
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

int ble_ppogatt_service_init(void) { return 0; }

BT_CONN_CB_DEFINE(prv_ppogatt_conn_callbacks) = {
    .connected = prv_on_connected,
    .security_changed = prv_on_security_changed,
};
