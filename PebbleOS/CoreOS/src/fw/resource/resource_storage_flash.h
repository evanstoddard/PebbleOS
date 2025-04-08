/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_storage_flash.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef resource_storage_flash_h
#define resource_storage_flash_h

#include "resource_storage_impl.h"

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
ResourceStoreImpl *resource_storage_flash_impl(void);

#ifdef __cplusplus
}
#endif
#endif /* resource_storage_flash_h */
