/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_storage.h
 * @author Evan Stoddard
 * @brief Module for fetching resources from storage
 */

#ifndef resource_storage_h
#define resource_storage_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
 * @brief Initialize resource storage module
 */
void resource_storage_init(void);

/**
 * @brief [TODO:description]
 *
 * @param app_num [TODO:parameter]
 * @param resource_id [TODO:parameter]
 * @param entry [TODO:parameter]
 * @return [TODO:return]
 */
bool resource_storage_get_resource(uint32_t app_num, uint32_t resource_id, ResourceStorageEntry *entry);

/**
 * @brief [TODO:description]
 *
 * @param entry [TODO:parameter]
 * @param offset [TODO:parameter]
 * @param dst [TODO:parameter]
 * @param size [TODO:parameter]
 * @return [TODO:return]
 */
size_t resource_storage_read_bytes(ResourceStorageEntry *entry, uint32_t offset, void *dst, size_t size);

#ifdef __cplusplus
}
#endif
#endif /* resource_storage_h */
