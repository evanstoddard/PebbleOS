/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_bt.h
 * @author Evan Stoddard
 * @brief Pebble-specific Bluetooth identifiers (UUIDs, service numbers) and
 * helper macros for constructing 128-bit UUIDs from the Pebble Base UUID.
 *
 * The Pebble Base UUID is: XXXXXXXX-328E-0FBB-C642-1AA6699BDADA
 *
 * Also see https://pebbletechnology.atlassian.net/wiki/display/DEV/Pebble+GATT+Services
 */

#ifndef pebble_bt_h
#define pebble_bt_h

#include <zephyr/bluetooth/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

//! Pebble Bluetooth SIG registered 16-bit UUID (Pebble Smartwatch Service)
#define PEBBLE_BT_PAIRING_SERVICE_UUID_16BIT 0xFED9

//! Expand a 32-bit (or 16-bit) Pebble UUID into the full 128-bit Pebble Base
//! UUID using Zephyr's BT_UUID_128_ENCODE layout.
//! Produces comma-separated little-endian bytes suitable for BT_UUID_INIT_128,
//! BT_UUID_DECLARE_128, and BT_DATA_BYTES.
//!
//! Pebble Base UUID: XXXXXXXX-328E-0FBB-C642-1AA6699BDADA
#define PEBBLE_BT_UUID_EXPAND(value) \
  BT_UUID_128_ENCODE(value, 0x328E, 0x0FBB, 0xC642, 0x1AA6699BDADA)

//! PPoGATT (Pebble Protocol over GATT) service and characteristics
#define PEBBLE_BT_PPOGATT_SERVICE_UUID_32BIT                       0x10000000
#define PEBBLE_BT_PPOGATT_DATA_CHARACTERISTIC_UUID_32BIT           0x10000001
#define PEBBLE_BT_PPOGATT_META_CHARACTERISTIC_UUID_32BIT           0x10000002

//! PPoGATT watch-as-server service and characteristics (reversed roles for
//! Android phones with broken GATT server APIs)
#define PEBBLE_BT_PPOGATT_WATCH_SERVER_SERVICE_UUID_32BIT          0x30000003
#define PEBBLE_BT_PPOGATT_WATCH_SERVER_DATA_CHARACTERISTIC_UUID_32BIT  0x30000004
#define PEBBLE_BT_PPOGATT_WATCH_SERVER_META_CHARACTERISTIC_UUID_32BIT  0x30000005
#define PEBBLE_BT_PPOGATT_WATCH_SERVER_DATA_WR_CHARACTERISTIC_UUID_32BIT 0x30000006

//! App Launch service and characteristic
#define PEBBLE_BT_APP_LAUNCH_SERVICE_UUID_32BIT                    0x20000000
#define PEBBLE_BT_APP_LAUNCH_CHARACTERISTIC_UUID_32BIT             0x20000001

//! Convenience: Pairing service as a Zephyr bt_uuid pointer (16-bit)
#define PEBBLE_BT_PAIRING_SERVICE_UUID \
  BT_UUID_DECLARE_16(PEBBLE_BT_PAIRING_SERVICE_UUID_16BIT)

//! Convenience: PPoGATT service as a Zephyr bt_uuid pointer (128-bit)
#define PEBBLE_BT_PPOGATT_SERVICE_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(PEBBLE_BT_PPOGATT_SERVICE_UUID_32BIT))

//! Convenience: PPoGATT data characteristic as a Zephyr bt_uuid pointer
#define PEBBLE_BT_PPOGATT_DATA_CHARACTERISTIC_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(PEBBLE_BT_PPOGATT_DATA_CHARACTERISTIC_UUID_32BIT))

//! Convenience: PPoGATT meta characteristic as a Zephyr bt_uuid pointer
#define PEBBLE_BT_PPOGATT_META_CHARACTERISTIC_UUID \
  BT_UUID_DECLARE_128(PEBBLE_BT_UUID_EXPAND(PEBBLE_BT_PPOGATT_META_CHARACTERISTIC_UUID_32BIT))

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* pebble_bt_h */
