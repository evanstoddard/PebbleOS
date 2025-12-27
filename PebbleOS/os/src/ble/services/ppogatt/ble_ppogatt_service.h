/*
 * Copyright (C) Ovyl
 */

/**
 * @file ble_ppogatt_service.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ble_ppogatt_service_h
#define ble_ppogatt_service_h

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

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
int ble_ppogatt_service_init(void);

/**
 * @brief [TODO:description]
 *
 * @return [TODO:return]
 */
int ble_ppogatt_fetch_meta_data(void);

/**
 * @brief [TODO:description]
 *
 * @param buf [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
int ble_ppogatt_write_data(const void *buf, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif /* ble_ppogatt_service_h */
