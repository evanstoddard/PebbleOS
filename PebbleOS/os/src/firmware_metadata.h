/*
 * Copyright (C) Ovyl
 */

/**
 * @file firmware_metadata.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef firmware_metadata_h
#define firmware_metadata_h

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum FirmwareMetadataPlatform {
  FirmwareMetadataPlatformUnknown = 0,
  FirmwareMetadataPlatformPebbleOneEV1 = 1,
  FirmwareMetadataPlatformPebbleOneEV2 = 2,
  FirmwareMetadataPlatformPebbleOneEV2_3 = 3,
  FirmwareMetadataPlatformPebbleOneEV2_4 = 4,
  FirmwareMetadataPlatformPebbleOnePointFive = 5,
  FirmwareMetadataPlatformPebbleTwoPointZero = 6,
  FirmwareMetadataPlatformPebbleSnowyEVT2 = 7,
  FirmwareMetadataPlatformPebbleSnowyDVT = 8,
  FirmwareMetadataPlatformPebbleSpaldingEVT = 9,
  FirmwareMetadataPlatformPebbleBobbyDVT = 10,
  FirmwareMetadataPlatformPebbleSpalding = 11,
  FirmwareMetadataPlatformPebbleSilkEVT = 12,
  FirmwareMetadataPlatformPebbleRobertEVT = 13,
  FirmwareMetadataPlatformPebbleSilk = 14,
  FirmwareMetadataPlatformPebbleAsterix = 15,
  FirmwareMetadataPlatformPebbleObelixEVT = 16,
  FirmwareMetadataPlatformPebbleObelixDVT = 17,
  FirmwareMetadataPlatformPebbleObelixPVT = 18,

  FirmwareMetadataPlatformPebbleOneBigboard = 0xff,
  FirmwareMetadataPlatformPebbleOneBigboard2 = 0xfe,
  FirmwareMetadataPlatformPebbleSnowyBigboard = 0xfd,
  FirmwareMetadataPlatformPebbleSnowyBigboard2 = 0xfc,
  FirmwareMetadataPlatformPebbleSpaldingBigboard = 0xfb,
  FirmwareMetadataPlatformPebbleSilkBigboard = 0xfa,
  FirmwareMetadataPlatformPebbleRobertBigboard = 0xf9,
  FirmwareMetadataPlatformPebbleSilkBigboard2 = 0xf8,
  FirmwareMetadataPlatformPebbleRobertBigboard2 = 0xf7,
  FirmwareMetadataPlatformPebbleSilkFlint = 0xf6,
  FirmwareMetadataPlatformPebbleSnowyEmery = 0xf5,
  FirmwareMetadataPlatformPebbleObelixBigboard = 0xf4,
  FirmwareMetadataPlatformPebbleObelixBigboard2 = 0xf3,
} FirmwareMetadataPlatform;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* firmware_metadata_h */
