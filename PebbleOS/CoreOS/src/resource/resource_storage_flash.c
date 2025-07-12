/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_storage_flash.c
 * @author Evan Stoddard
 * @brief
 */

#include "resource_storage_flash.h"

#include <zephyr/kernel.h>

#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

#include <zephyr/sys/crc.h>

#include <zephyr/logging/log.h>

#include "resource_storage_impl.h"

#include "flash_map/flash_map.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(resource_storage_flash, LOG_LEVEL_DBG);

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct
{
    bool initialized;
    ResourceManifest system_manifest;
    bool store_valid;
    const struct flash_area *fa;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static void prv_validate_system_store(void)
{
    int ret = flash_area_read(prv_inst.fa, 0, &prv_inst.system_manifest, sizeof(ResourceManifest));

    if (ret != 0)
    {
        LOG_ERR("Unable to read system storage manifest.");
        return;
    }

    LOG_DBG("Number of resources: %u", prv_inst.system_manifest.num_resources);

    uint32_t calculated_crc32 = 0;
    for (size_t i = 0; i < prv_inst.system_manifest.num_resources; i++)
    {
        ResourceTableEntry entry = {0};

        off_t offset = sizeof(ResourceManifest) + (sizeof(ResourceTableEntry) * i);

        ret = flash_area_read(prv_inst.fa, offset, &entry, sizeof(ResourceTableEntry));

        if (ret != 0)
        {
            LOG_ERR("Unable to read table entry.");
            return;
        }

        calculated_crc32 = crc32_ieee_update(calculated_crc32, (const uint8_t *)&entry, sizeof(ResourceTableEntry));
    }

    LOG_DBG("Calculated CRC32: 0x%08X", calculated_crc32);
    LOG_DBG("Expected CRC32: 0x%08X", prv_inst.system_manifest.version.crc32);

    prv_inst.store_valid = (calculated_crc32 == prv_inst.system_manifest.version.crc32);
}

/**
 * @brief [TODO:description]
 */
static void prv_init(void)
{
    if (prv_inst.initialized == true)
    {
        return;
    }

    prv_inst.fa = FLASH_MAP_SYSTEM_RESOURCES_PARTITION;

    prv_validate_system_store();

    prv_inst.initialized = true;
}

/**
 * @brief [TODO:description]
 *
 * @param app_num [TODO:parameter]
 * @return [TODO:return]
 */
static bool prv_store_valid(uint32_t app_num)
{
    return ((app_num == SYSTEM_APP) && prv_inst.store_valid && prv_inst.initialized);
}

/**
 * @brief [TODO:description]
 *
 * @param app_num [TODO:parameter]
 * @param resource_id [TODO:parameter]
 * @param entry [TODO:parameter]
 * @return [TODO:return]
 */
static bool prv_get_resource(uint32_t app_num, uint32_t resource_id, ResourceStorageEntry *entry)
{
    if (prv_inst.initialized == false || prv_inst.store_valid == false)
    {
        return false;
    }

    if (app_num != SYSTEM_APP)
    {
        return false;
    }

    if (resource_id >= prv_inst.system_manifest.num_resources)
    {
        return false;
    }

    ResourceTableEntry table_entry = {0};
    off_t offset = sizeof(ResourceManifest) + (resource_id * sizeof(ResourceTableEntry));
    int ret = flash_area_read(prv_inst.fa, offset, &table_entry, sizeof(table_entry));
    if (ret != 0)
    {
        return false;
    }

    uint32_t payload_offset = sizeof(ResourceManifest) +
                              (sizeof(ResourceTableEntry) * prv_inst.system_manifest.num_resources) +
                              table_entry.offset;
    (void)payload_offset;

    *entry = (ResourceStorageEntry){.offset = table_entry.offset,
                                    .id = table_entry.resource_id,
                                    .length = table_entry.length,
                                    .impl = resource_storage_flash_impl()};

    // TODO: CRC32 Validation

    return true;
}

/**
 * @brief [TODO:description]
 *
 * @param entry [TODO:parameter]
 * @param dst [TODO:parameter]
 * @param max_size [TODO:parameter]
 * @return [TODO:return]
 */
static size_t prv_read_resource(ResourceStorageEntry *entry, uint32_t offset, void *dst, size_t max_size)
{
    uint32_t end = entry->offset + entry->length;

    uint32_t payload_offset = sizeof(ResourceManifest) +
                              (sizeof(ResourceTableEntry) * prv_inst.system_manifest.num_resources) + entry->offset;
    uint32_t offset_final = payload_offset + offset;

    size_t max_length = entry->length - offset;

    if (end < offset_final)
    {
        return 0;
    }

    size_t actual_size = max_length < max_size ? max_length : max_size;

    int ret = flash_area_read(prv_inst.fa, offset_final, dst, actual_size);

    if (ret != 0)
    {
        return 0;
    }

    return actual_size;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

ResourceStoreImpl *resource_storage_flash_impl(void)
{
    static ResourceStoreImpl impl = {
        .init = prv_init, .store_valid = prv_store_valid, .get_resource = prv_get_resource, .read = prv_read_resource};

    return &impl;
}
