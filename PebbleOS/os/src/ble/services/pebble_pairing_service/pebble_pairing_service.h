/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_pairing_service.h
 * @author Evan Stoddard
 * @brief Device hosted BLE service coordinate device pairing
 */

#ifndef pebble_pairing_service_h
#define pebble_pairing_service_h

#ifdef __cplusplus
extern "C" {
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

/**
 * @brief Initialize Pebble pairing service
 *
 * @return Returns 0 on success
 */
int pebble_pairing_service_init(void);

#ifdef __cplusplus
}
#endif
#endif /* pebble_pairing_service_h */
