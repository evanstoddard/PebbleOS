/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource.h
 * @author Evan Stoddard
 * @brief Module for fetching resources
 */

#ifndef resource_h
#define resource_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "resource_storage.h"

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
 * @param app_num [TODO:parameter]
 * @return [TODO:return]
 */
bool resource_init_app(uint32_t app_num);

bool resource_is_valid(uint32_t app_num, uint32_t resource_id);

size_t resource_size(uint32_t app_num, uint32_t resource_id);

size_t resource_read_bytes(uint32_t app_num, uint32_t resource_id, void *dst, size_t max_bytes);

#ifdef __cplusplus
}
#endif
#endif /* resource_h */
