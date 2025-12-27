/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_advertising.c
 * @author Evan Stoddard
 * @brief
 */

#include "ble_advertising.h"

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define BLE_ADV_MIN_ADVERTISE_INTERVAL (800)
#define BLE_ADV_MAX_ADVERTISE_INTERVAL (801)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef ManufacturerSpecificData_t
 * @brief [TODO:description]
 *
 */
typedef struct ManufacturerSpecificData_t {
  uint16_t company_id;

  uint8_t payload_type;

  char serial_number[12];

  uint8_t hw_platform;

  uint8_t color;
  uint8_t fw_major;
  uint8_t fw_minor;
  uint8_t fw_patch;

  union {
    uint8_t flags;

    struct {
      bool is_running_recovery_firmware : 1;
      bool is_first_use : 1;
    };
  };

} __attribute__((__packed__)) ManufacturerSpecificData_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

static const struct bt_le_adv_param *prv_adv_params =
    BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY),
                    BLE_ADV_MIN_ADVERTISE_INTERVAL, BLE_ADV_MAX_ADVERTISE_INTERVAL, NULL);

static const struct bt_data prv_advertising_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),

    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xD9, 0xFE),  // 0xFED9 little-endian

    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/**
 * @brief [TODO:description]
 */
static struct {
  struct k_work adv_worker;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param work [TODO:parameter]
 */
static void prv_advertising_work_handler(struct k_work *work) {
  // TODO: Dynamically fetch manufacturing data
  static ManufacturerSpecificData_t mfg_data = {0};
  mfg_data.company_id = 0x0154;
  mfg_data.payload_type = 0x0;
  mfg_data.hw_platform = 0x2;
  mfg_data.color = 0x0C;
  mfg_data.flags = 0x0;

  static const struct bt_data sd[] = {
      BT_DATA(BT_DATA_MANUFACTURER_DATA, &mfg_data, sizeof(mfg_data))};

  bt_le_adv_start(prv_adv_params, prv_advertising_data, ARRAY_SIZE(prv_advertising_data), sd,
                  ARRAY_SIZE(sd));
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void ble_advertising_init(void) {
  k_work_init(&prv_inst.adv_worker, prv_advertising_work_handler);
}

void ble_advertising_begin(void) {
  k_work_submit(&prv_inst.adv_worker);
}
