/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_storage.c
 * @author Evan Stoddard
 * @brief
 */

#include "resource_storage.h"
#include "resource_storage_flash.h"
#include "resource_storage_impl.h"

#include <zephyr/logging/log.h>

#include <stdbool.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(resource_storage, LOG_LEVEL_DBG);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private instance
 */
static struct
{
    bool initialized;
} prv_inst;

/**
 * @brief Static array of implementation getters
 */
static ResourceStoreImplGetter prv_storage_impl_list[] = {resource_storage_flash_impl};

static size_t prv_num_storage_impls = sizeof(prv_storage_impl_list) / sizeof(ResourceStoreImplGetter);

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void resource_storage_init(void)
{
    if (prv_inst.initialized == true)
    {
        return;
    }

    // Initialize each storage implementation
    for (size_t i = 0; i < prv_num_storage_impls; i++)
    {
        ResourceStoreImpl *impl = prv_storage_impl_list[i]();

        impl->init();
    }

    prv_inst.initialized = true;
}

bool resource_storage_get_resource(uint32_t app_num, uint32_t resource_id, ResourceStorageEntry *entry)
{
    *entry = (ResourceStorageEntry){0};

    for (size_t i = 0; i < prv_num_storage_impls; i++)
    {
        ResourceStoreImpl *impl = prv_storage_impl_list[i]();

        bool ret = impl->get_resource(app_num, resource_id, entry);

        if (ret)
        {
            return true;
        }
    }

    return false;
}

size_t resource_storage_read_bytes(ResourceStorageEntry *entry, uint32_t offset, void *dst, size_t size)
{
    if (entry->impl == NULL)
    {
        return 0;
    }

    return entry->impl->read(entry, offset, dst, size);
}
