/*
 * Copyright (C) Ovyl
 */

/**
 * @file flash_map.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef flash_map_h
#define flash_map_h

#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define FLASH_MAP_SYSTEM_RESOURCES_PARTITION FIXED_PARTITION(system_resources)

#define FLASH_MAP_SYSTEM_RESOURCES_PARTITION_SIZE FIXED_PARTITION_SIZE(system_resources)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* flash_map_h */
