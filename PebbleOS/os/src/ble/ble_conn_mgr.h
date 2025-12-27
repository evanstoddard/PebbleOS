/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_conn_mgr.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ble_conn_mgr_h
#define ble_conn_mgr_h

#include <stdint.h>

#include <zephyr/sys/slist.h>

#include <zephyr/bluetooth/bluetooth.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef ble_client_t
 * @brief [TODO:description]
 *
 */
typedef struct ble_client_t {
  sys_snode_t node;

  const char *client_name;

  const struct bt_uuid *service_uuid;

  /* Array of characteristic UUIDs to find within the service */
  const struct bt_uuid **char_uuids;

  /* Number of characteristics in char_uuids array */
  uint8_t num_chars;

  /* Called when service is discovered with all required characteristics.
   * char_handles array is ordered to match char_uuids.
   * service_end_handle is the last handle in the service (for CCCD discovery). */
  void (*on_service_discovered)(uint16_t *char_handles, uint16_t service_end_handle);

  /* Called when service is removed or connection lost */
  void (*on_service_removed)(void);

  /* Called when all handles should be invalidated (e.g., disconnect, re-discovery) */
  void (*on_invalidate_all)(void);

  /* Returns true if this client owns the given characteristic handle */
  bool (*can_handle)(uint16_t handle);

  /* Called on GATT notification/indication for a characteristic this client owns */
  void (*on_notify)(uint16_t handle, const uint8_t *data, uint16_t len);

  /* Called on GATT read response for a characteristic this client owns */
  void (*on_read)(uint16_t handle, const uint8_t *data, uint16_t len, uint8_t err);

  /* Called on GATT write response for a characteristic this client owns */
  void (*on_write)(uint16_t handle, uint8_t err);

  /* Called when subscription (notify/indicate) completes */
  void (*on_subscribe)(uint16_t handle, uint8_t err);
} ble_client_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize BLE comm manager
 *
 * @return
 */
int ble_conn_mgr_init(void);

/**
 * @brief [TODO:description]
 *
 * @param client [TODO:parameter]
 */
void ble_conn_mgr_register_client(ble_client_t *client);

#ifdef __cplusplus
}
#endif
#endif /* ble_conn_mgr_h */
