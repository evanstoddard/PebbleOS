/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_device.h
 * @author Evan Stoddard
 * @brief Various defines for target platform
 */

#ifndef pebble_device_h
#define pebble_device_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DEVICE_TINTIN
#include "device_tintin.h"
#elif CONFIG_DEVICE_BLANCA
#include "device_blanca.h"
#else
#error "Device type not defined or supported."
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* pebble_device_h */