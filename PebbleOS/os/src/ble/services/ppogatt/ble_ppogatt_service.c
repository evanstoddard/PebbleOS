/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_ppogatt_service.c
 * @author Evan Stoddard
 * @brief
 */

#include "ble_ppogatt_service.h"

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

/*****************************************************************************
 * BLE Service Bindings
 *****************************************************************************/

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
static ssize_t prv_on_meta_read(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr, void *buf,
                                uint16_t len, uint16_t offset);

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
static ssize_t prv_on_data_write(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len, uint16_t offset,
                                 uint8_t flags);

/**
 * @brief [TODO:description]
 *
 * @param attr [TODO:parameter]
 * @param value [TODO:parameter]
 */
static void prv_on_data_ccc_changed(const struct bt_gatt_attr *attr,
                                    uint16_t value);

BT_GATT_SERVICE_DEFINE(
    prv_ppogatt_svc,
    /* Primary Service Declaration */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_PPOGATT_SVC),

    /* Data Characteristic: Write Without Response + Notify */
    BT_GATT_CHARACTERISTIC(BT_UUID_PPOGATT_DATA,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP |
                               BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_WRITE, NULL, /* No read callback */
                           prv_on_data_write,        /* Write callback */
                           NULL),                    /* No user data */
    BT_GATT_CCC(prv_on_data_ccc_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Meta Characteristic: Read only */
    BT_GATT_CHARACTERISTIC(BT_UUID_PPOGATT_META, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           prv_on_meta_read, /* Read callback */
                           NULL,             /* No write callback */
                           NULL),            /* No user data */
);

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

static ssize_t prv_on_meta_read(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr, void *buf,
                                uint16_t len, uint16_t offset) {
  LOG_INF("Requesting metadata.");
  return -ENOTSUP;
}

static ssize_t prv_on_data_write(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len, uint16_t offset,
                                 uint8_t flags) {
  LOG_HEXDUMP_INF(buf, len, "Writing data:");
  return -ENOTSUP;
}

static void prv_on_data_ccc_changed(const struct bt_gatt_attr *attr,
                                    uint16_t value) {
  LOG_INF("Characteristic changed: 0x%04X", value);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/
