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
 * @brief Initialize BLE comm manager
 *
 * @return
 */
int ble_conn_mgr_init(void);

#ifdef __cplusplus
}
#endif
#endif /* ble_conn_mgr_h */
