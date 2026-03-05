/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_pairing_service_private.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef pebble_pairing_service_private_h
#define pebble_pairing_service_private_h

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/bluetooth/gatt.h>

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
 * @brief Potential error codes for Pebble pairing service
 */
typedef enum {
  PebblePairingServiceGATTError_UnknownCommandID = 0x80,
  PebblePairingServiceGATTError_ConnParamsInvalidRemoteDesiredState,
  PebblePairingServiceGATTError_ConnParamsMinSlotsTooSmall,
  PebblePairingServiceGATTError_ConnParamsMinSlotsTooLarge,
  PebblePairingServiceGATTError_ConnParamsMaxSlotsTooLarge,
  PebblePairingServiceGATTError_ConnParamsSupervisionTimeoutTooSmall,
  PebblePairingServiceGATTError_DeviceDoesNotSupportPLE,
} PebblePairingServiceGATTError_t;

/**
 * @typedef PebblePairingServiceConnectivityStatus_t
 * @brief Connectivity status payload
 *
 */
typedef struct PebblePairingServiceConnectivityStatus_t {
  union {
    struct {
      //! true if the device that is reading the status is connected (always
      //! true)
      bool ble_is_connected : 1;
      //! true if the device that is reading the status is bonded, false if not
      bool ble_is_bonded : 1;
      //! true if the current LE link is encrypted, false if not
      bool ble_is_encrypted : 1;
      //! true if the watch has a bonding to a gateway (LE-based).
      bool has_bonded_gateway : 1;
      //! true if the watch supports writing the "Don't send slave security
      //! request"
      bool supports_pinning_without_security_request : 1;
      //! true if the reversed ppogatt was enabled at the time of bonding
      bool is_reversed_ppogatt_enabled : 1;

      //! Reserved, leave zero for future use.
      uint32_t rsvd : 18;

      //! The error of the last pairing process or all zeroes, if no pairing
      //! process has completed or when there were no errors. Also see BT
      //! Spec 4.2, Vol 3, Part H, 3.5.5 Pairing Failed.
      uint8_t last_pairing_result;
    };
    uint8_t bytes[4];
  };
} __attribute__((__packed__)) PebblePairingServiceConnectivityStatus_t;

/**
 * @typedef PairingTriggerRequestData_t
 * @brief Pairing trigger request data containing flags for controlling pairing
 * behavior and connection security options.
 *
 */
typedef struct PairingTriggerRequestData_t {
  bool should_pin_address : 1;

  //! @note Not available in Bluetopia/cc2564x implementation
  //! This flag and should_force_slave_security_request are mutually exclusive!
  bool no_slave_security_request : 1;

  //! @note Not available in Bluetopia/cc2564x implementation
  //! This flag and no_slave_security_request are mutually exclusive!
  bool should_force_slave_security_request : 1;

  //! @note Not available in Bluetopia/cc2564x implementation
  //! Flag to indicate that when re-pairing this device, the re-pairing should be accepted
  //! automatically for this remote device (matching IRK or matching identity address).
  //! @note This is a work-around for an Android 4.4.x bug. This opens up a security hole :( where
  //! a phone could pretend to be the "trusted" phone and pair w/o the user even knowing about it.
  //! @see https://pebbletechnology.atlassian.net/browse/PBL-39369
  bool should_auto_accept_re_pairing : 1;

  //! @note Not available in Bluetopia/cc2564x implementation
  //! Flag to indicate that the PPoGATT server/client roles should be reversed to support the
  //! connected phone. Some older Android phones' GATT service API is completely busted. For those
  //! poor phones, this bit is set before pairing. The Pebble includes a "reversed" PPoGATT service
  //! that the phone app can connect to as GATT client, but this service only works if this bit
  //! gets set *before pairing*. This is a security measure: 1. to prevent non-paired devices from
  //! talking to the "reversed" PPoGATT service. 2. to prevent non-Pebble apps on paired phone that
  //! does support normal PPoGATT from connecting to the "reversed" PPoGATT service.
  //! @see ppogatt_emulated_server_wa.c
  //! @see https://pebbletechnology.atlassian.net/browse/PBL-39634
  bool is_reversed_ppogatt_enabled : 1;
} __attribute__((__packed__)) PairingTriggerRequestData_t;

/**
 * @typedef PebblePairingServiceConnParamSet_t
 * @brief A single set of BLE connection parameters used by the connection
 * parameter manager.
 *
 */
typedef struct PebblePairingServiceConnParamSet_t {
  //! interval_min_ms / 1.25 msec – valid range: 7.5 msec to 4 seconds
  uint16_t interval_min_1_25ms;

  //! (interval_max_ms - interval_min_ms) / 1.25 msec
  //! @note To fit the parent struct in the minimum GATT MTU, this field is a delta and only one
  //! byte instead of the uint16_t that the BT spec uses.
  uint8_t interval_max_delta_1_25ms;

  //! Slave latency (in number of connection events)
  //! @note To fit the parent struct in the minimum GATT MTU, this field is only one byte instead
  //! of the uint16_t that the BT spec uses.
  uint8_t slave_latency_events;

  //! Supervision Timeout / 30 msec – valid range: 100 msec to 32 seconds. To fit this into one
  //! byte and to fit the parent struct in the minimum GATT MTU, the increments is not the standard
  //! 10msec!
  uint8_t supervision_timeout_30ms;
} __attribute__((__packed__)) PebblePairingServiceConnParamSet_t;

/**
 * @typedef PebblePairingServiceConnParamsReadNotif_t
 * @brief The connection parameters settings, with respect to the connection to
 * the device reading them.
 *
 */
typedef struct PebblePairingServiceConnParamsReadNotif_t {
  //! Capability bits. Reserved for future use.
  uint8_t packet_length_extension_supported : 1;
  uint8_t rsvd : 7;

  //! Current interval / 1.25 msec – valid range: 7.5 msec to 4 seconds
  uint16_t current_interval_1_25ms;

  //! Current Slave latency (in number of connection events) – actual max is 0x01F3, but in
  //! practice values are much lower.
  uint16_t current_slave_latency_events;

  //! Current Supervision Timeout / 10 msec – valid range: 100 msec to 32 seconds.
  uint16_t current_supervision_timeout_10ms;
} __attribute__((__packed__)) PebblePairingServiceConnParamsReadNotif_t;

/**
 * @brief Command IDs for connection parameter write operations
 */
typedef enum {
  //! Allows phone to change connection parameter set and take over control of parameter management
  PebblePairingServiceConnParamsWriteCmd_SetRemoteParamMgmtSettings = 0x00,
  //! Issues a connection parameter change request if the watch is not in the desired state
  PebblePairingServiceConnParamsWriteCmd_SetRemoteDesiredState = 0x01,
  //! Controls settings for BLE 4.2 Packet Length Extension feature
  PebblePairingServiceConnParamsWriteCmd_EnablePacketLengthExtension = 0x02,
  //! If written to disables Dialog BLE sleep mode (safeguard against PBL-39777 in case it affects
  //! more watches in the future)
  PebblePairingServiceConnParamsWriteCmd_InhibitBLESleep = 0x03,
  PebblePairingServiceConnParamsWriteCmdCount,
} PebblePairingServiceConnParamsWriteCmd_t;

/**
 * @typedef PebblePairingServiceRemoteParamMgmtSettings_t
 * @brief Settings that allow the remote device to take over management of BLE
 * connection parameters.
 *
 */
typedef struct PebblePairingServiceRemoteParamMgmtSettings_t {
  //! If false/zero, Pebble should manage the connection parameters. If true/one, Pebble should
  //! NOT manage the connection parameters. In this mode, Pebble will never request a
  //! connection parameter change.
  bool is_remote_device_managing_connection_parameters : 1;
  uint8_t rsvd : 7;
  //! Optional. Current parameters sets used by Pebble's Connection Parameter manager.
  PebblePairingServiceConnParamSet_t connection_parameter_sets[];
} __attribute__((__packed__)) PebblePairingServiceRemoteParamMgmtSettings_t;

/**
 * @typedef PebblePairingServiceRemoteDesiredState_t
 * @brief The remote device's desired connection response time state.
 *
 */
typedef struct PebblePairingServiceRemoteDesiredState_t {
  //! The desired ResponseTime as desired by the remote device.  The remote end can set this
  //! value to a faster mode when it's about to transfer/receive a lot of data. For example,
  //! when a lot of BlobDB operations are queued up, the watch doesn't know how much data is
  //! queued up on the remote end. In this case, the remote could write "ResponseTimeMin" so
  //! increase the speed temporarily. It's the remote end's responsibility to reset this to
  //! ResponseTimeMax when the bulk transfer is done.  As a safety measure, the watch is will
  //! reset it back to ResponseTimeMax after 5 minutes.  In case the phone app still wants to
  //! keep a particular desired ResponseTime, the phone app is responsible for making sure to
  //! write the value again before the 5 minute timer expires.
  uint8_t state : 2;

  uint8_t rsvd : 6;
} __attribute__((__packed__)) PebblePairingServiceRemoteDesiredState_t;

/**
 * @typedef PebblePairingServicePacketLengthExtension_t
 * @brief Request payload for enabling BLE 4.2 Packet Length Extension.
 *
 */
typedef struct PebblePairingServicePacketLengthExtension_t {
  uint8_t trigger_ll_length_req : 1;
  uint8_t rsvd : 7;
} __attribute__((__packed__)) PebblePairingServicePacketLengthExtension_t;

/**
 * @typedef PebblePairingServiceInhibitBLESleep_t
 * @brief Request payload for inhibiting BLE sleep mode.
 *
 */
typedef struct PebblePairingServiceInhibitBLESleep_t {
  uint8_t rsvd;  // for future use
} __attribute__((__packed__)) PebblePairingServiceInhibitBLESleep_t;

/**
 * @typedef PebblePairingServiceConnParamsWrite_t
 * @brief The connection parameters settings, with respect to the connection to
 * the device writing them. Contains a command ID and a union of command-specific
 * payloads.
 *
 */
typedef struct PebblePairingServiceConnParamsWrite_t {
  PebblePairingServiceConnParamsWriteCmd_t cmd : 8;
  union __attribute__((__packed__)) {
    //! Valid iff cmd == PebblePairingServiceConnParamsWriteCmd_SetRemoteParamMgmtSettings
    PebblePairingServiceRemoteParamMgmtSettings_t remote_param_mgmt_settings;

    //! Valid iff cmd == PebblePairingServiceConnParamsWriteCmd_SetRemoteDesiredState
    PebblePairingServiceRemoteDesiredState_t remote_desired_state;

    //! Valid iff cmd == PebblePairingServiceConnParamsWriteCmd_EnablePacketLengthExtension
    PebblePairingServicePacketLengthExtension_t ple_req;

    //! Valid iff cmd == PebblePairingServiceConnParamsWriteCmd_InhibitBLESleep
    PebblePairingServiceInhibitBLESleep_t ble_sleep;
  };
} __attribute__((__packed__)) PebblePairingServiceConnParamsWrite_t;
/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* pebble_pairing_service_private_h */
