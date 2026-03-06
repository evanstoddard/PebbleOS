/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file ppogatt_client.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef ppogatt_client_h
#define ppogatt_client_h

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
 * @brief Initialize PPoGATT GATT Client
 *
 * @return Returns 0 on success
 */
int ppogatt_client_init(void);

#ifdef __cplusplus
}
#endif
#endif /* ppogatt_client_h */
