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
#include "services/ppogatt_client/ppogatt_client.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(ble_conn_mgr);

#define MAX_CHARS_PER_CLIENT 8

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Module instance state
 */
static struct {
  struct bt_conn *conn;

  sys_slist_t clients;

  struct {
    ble_client_t *current_client;

    struct bt_gatt_discover_params params;

    uint16_t service_start_handle;
    uint16_t service_end_handle;

    uint16_t resume_handle;

    uint16_t char_handles[MAX_CHARS_PER_CLIENT];
    uint8_t chars_found;
  } discovery;

} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Find a registered client by service UUID
 *
 * @param uuid Service UUID to match
 * @return Pointer to matching client, or NULL if not found
 */
static ble_client_t *prv_find_client_by_service_uuid(const struct bt_uuid *uuid) {
  ble_client_t *client;

  SYS_SLIST_FOR_EACH_CONTAINER(&prv_inst.clients, client, node) {
    if (bt_uuid_cmp(client->service_uuid, uuid) == 0) {
      return client;
    }
  }

  return NULL;
}

/**
 * @brief Match a discovered characteristic UUID to the current client's expected UUIDs
 *
 * @param uuid Characteristic UUID to match
 * @return Index into client's char_uuids array, or -1 if not found
 */
static int prv_find_char_index(const struct bt_uuid *uuid) {
  ble_client_t *client = prv_inst.discovery.current_client;

  if (client == NULL) {
    return -1;
  }

  for (uint8_t i = 0; i < client->num_chars; i++) {
    if (bt_uuid_cmp(client->char_uuids[i], uuid) == 0) {
      return i;
    }
  }

  return -1;
}

/**
 * @brief GATT discovery callback
 *
 * @param conn Connection object
 * @param attr Discovered attribute, or NULL if discovery complete
 * @param params Discovery parameters
 * @return BT_GATT_ITER_STOP or BT_GATT_ITER_CONTINUE
 */
static uint8_t prv_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               struct bt_gatt_discover_params *params);

/**
 * @brief Start characteristic discovery for the current client's service
 *
 * @param conn Connection object
 * @return 0 on success, negative error code on failure
 */
static int prv_start_char_discovery(struct bt_conn *conn) {
  prv_inst.discovery.params.func = prv_discover_cb;
  prv_inst.discovery.params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
  prv_inst.discovery.params.start_handle = prv_inst.discovery.service_start_handle;
  prv_inst.discovery.params.end_handle = prv_inst.discovery.service_end_handle;
  prv_inst.discovery.params.uuid = NULL;

  prv_inst.discovery.chars_found = 0;
  memset(prv_inst.discovery.char_handles, 0, sizeof(prv_inst.discovery.char_handles));

  return bt_gatt_discover(conn, &prv_inst.discovery.params);
}

/**
 * @brief Start primary service discovery
 *
 * @param conn Connection object
 * @return 0 on success, negative error code on failure
 */
static int prv_start_service_discovery(struct bt_conn *conn) {
  prv_inst.discovery.params.func = prv_discover_cb;
  prv_inst.discovery.params.type = BT_GATT_DISCOVER_PRIMARY;
  prv_inst.discovery.params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
  prv_inst.discovery.params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
  prv_inst.discovery.params.uuid = NULL;

  prv_inst.discovery.current_client = NULL;
  prv_inst.discovery.resume_handle = 0;

  return bt_gatt_discover(conn, &prv_inst.discovery.params);
}

/**
 * @brief Resume service discovery from where we left off
 *
 * @param conn Connection object
 * @return 0 on success, negative error code on failure
 */
static int prv_resume_service_discovery(struct bt_conn *conn) {
  if (prv_inst.discovery.resume_handle == 0 ||
      prv_inst.discovery.resume_handle >= BT_ATT_LAST_ATTRIBUTE_HANDLE) {
    LOG_INF("No more services to discover");
    return 0;
  }

  prv_inst.discovery.params.func = prv_discover_cb;
  prv_inst.discovery.params.type = BT_GATT_DISCOVER_PRIMARY;
  prv_inst.discovery.params.start_handle = prv_inst.discovery.resume_handle;
  prv_inst.discovery.params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
  prv_inst.discovery.params.uuid = NULL;

  prv_inst.discovery.current_client = NULL;

  return bt_gatt_discover(conn, &prv_inst.discovery.params);
}

/**
 * @brief GATT discovery callback
 *
 * @param conn Connection object
 * @param attr Discovered attribute, or NULL if discovery complete
 * @param params Discovery parameters
 * @return BT_GATT_ITER_STOP or BT_GATT_ITER_CONTINUE
 */
static uint8_t prv_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               struct bt_gatt_discover_params *params) {
  if (attr == NULL) {
    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
      LOG_INF("Service discovery complete");
    } else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
      ble_client_t *client = prv_inst.discovery.current_client;

      if (client != NULL && prv_inst.discovery.chars_found == client->num_chars) {
        LOG_INF("All %d characteristics found for %s", client->num_chars, client->client_name);

        if (client->on_service_discovered != NULL) {
          client->on_service_discovered(prv_inst.discovery.char_handles,
                                        prv_inst.discovery.service_end_handle);
        }
      } else if (client != NULL) {
        LOG_WRN("Only found %d of %d characteristics for %s", prv_inst.discovery.chars_found,
                client->num_chars, client->client_name);
      }

      prv_inst.discovery.current_client = NULL;

      /* Resume service discovery to find services for other clients */
      int err = prv_resume_service_discovery(conn);

      if (err) {
        LOG_ERR("Failed to resume service discovery: %d", err);
      }
    }

    return BT_GATT_ITER_STOP;
  }

  if (params->type == BT_GATT_DISCOVER_PRIMARY) {
    struct bt_gatt_service_val *svc = (struct bt_gatt_service_val *)attr->user_data;

    ble_client_t *client = prv_find_client_by_service_uuid(svc->uuid);
    if (client != NULL) {
      LOG_INF("Found service for client: %s", client->client_name);

      prv_inst.discovery.current_client = client;
      prv_inst.discovery.service_start_handle = attr->handle + 1;
      prv_inst.discovery.service_end_handle = svc->end_handle;
      prv_inst.discovery.resume_handle = svc->end_handle + 1;

      int err = prv_start_char_discovery(conn);
      if (err) {
        LOG_ERR("Failed to start characteristic discovery: %d", err);
        prv_inst.discovery.current_client = NULL;
      }

      return BT_GATT_ITER_STOP;
    }

  } else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
    struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;

    int idx = prv_find_char_index(chrc->uuid);
    if (idx >= 0 && idx < MAX_CHARS_PER_CLIENT) {
      prv_inst.discovery.char_handles[idx] = chrc->value_handle;
      prv_inst.discovery.chars_found++;

      LOG_DBG("Found characteristic %d at handle 0x%04x", idx, chrc->value_handle);
    }
  }

  return BT_GATT_ITER_CONTINUE;
}

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

  // Kick of discovery after security has been elevated indicating bonded
  if (level >= BT_SECURITY_L2) {
    int ret = prv_start_service_discovery(conn);
    if (ret) {
      LOG_ERR("Failed to start service discovery: %d", ret);
    }
  }
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
  sys_slist_init(&prv_inst.clients);

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
  ppogatt_client_init();

  return 0;
}

void ble_conn_mgr_register_client(ble_client_t *client) {
  sys_slist_append(&prv_inst.clients, (sys_snode_t *)client);
}
