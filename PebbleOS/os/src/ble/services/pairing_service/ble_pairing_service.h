/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_pairing_service.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ble_pairing_service_h
#define ble_pairing_service_h

#include <stdbool.h>
#include <stdint.h>

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
 * @typedef PebblePairingServiceConnectivityStatus_t
 * @brief [TODO:description]
 *
 */
typedef struct PPSConnectivityStatus_t {
  union {
    struct {
      /**
       * @brief Bit 0: Always true
       */
      bool ble_is_connected : 1;

      /**
       * @brief Bit 1: Device is bonded
       */
      bool ble_is_bonded : 1;

      /**
       * @brief Bit 2: Link is encrypted
       */
      bool ble_is_encrypted : 1;

      /**
       * @brief Bit 3: Has LE-based bonded gateway
       */
      bool has_bonded_gateway : 1;

      /**
       * @brief Bit 4: Supports specific security mode
       */
      bool supports_pinning_without_security_request : 1;

      /**
       * @brief Bit 5: Reversed PPoGATT mode active
       */
      bool is_reversed_ppogatt_enabled : 1;

      /**
       * @brief Unused (0)
       */
      uint32_t rsvd : 18;

      /**
       * @brief Bits 24-31: Last pairing error code (0 == success)
       */
      uint8_t last_pairing_result;
    };
    uint8_t bytes[4];
  };
} __attribute__((__packed__)) PPSConnectivityStatus_t;

/**
 * @typedef PPSPairingTriggerRequestData_t
 * @brief [TODO:description]
 *
 */
typedef struct PPSPairingTriggerRequestData_t {
  bool should_pin_address : 1;
  bool no_slave_security_request : 1;
  bool should_force_slave_security_request : 1;
  bool should_auto_accept_re_pairing : 1;
  bool is_reversed_ppogatt_enabled : 1;
} __attribute__((__packed__)) PPSPairingTriggerRequestData_t;

/**
 * @typedef PPSConnectionParamsReadNotification_t
 * @brief [TODO:description]
 *
 */
typedef struct PPSConnectionParamsReadNotification_t {
  uint8_t packet_length_extension_supported : 1;
  uint8_t rsvd : 7;
  uint16_t current_interval_1_25ms;
  uint16_t current_slave_latency_events;
  uint16_t current_supervision_timeout_10ms;
} __attribute__((__packed__)) PPSConnectionParamsReadNotification_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

int ble_pairing_service_init(void);

#ifdef __cplusplus
}
#endif
#endif /* ble_pairing_service_h */
