/*
 * Copyright (C) Ovyl
 */

/**
 * @file flash_sim.c
 * @author Evan Stoddard
 * @brief A memory mapped flash simulator
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>

#include <stddef.h>
#include <string.h>

#include "flash_sim_posix_api.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define DT_DRV_COMPAT pebble_sim_flash

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

static int flash_init(const struct device *dev)
{
    flash_sim_data_t *data = (flash_sim_data_t *)dev->data;

    int ret = flash_sim_init(data);

    // Use `dev->config` to get pointer to memory mapped "flash"
    // This is why you should never assume `const` is actually constant...
    ((struct device *)dev)->config = data->mapped;

    return ret;
}

static int flash_sim_read(const struct device *dev, const off_t offset, void *data, const size_t len)
{
    uintptr_t src_ptr = (uintptr_t)(dev->config) + offset;
    void *src = (void *)src_ptr;

    memcpy(data, src, len);

    return 0;
}

static int flash_sim_write(const struct device *dev, const off_t offset, const void *data, const size_t len)
{
    uintptr_t src_ptr = (uintptr_t)(dev->config) + offset;
    void *src = (void *)src_ptr;

    memcpy(src, data, len);

    return 0;
}

static int flash_sim_erase(const struct device *dev, const off_t offset, const size_t len)
{
    // TODO: See previous todo...
    return 0;
}

static int flash_sim_get_size(const struct device *dev, uint64_t *size)
{
    // TODO: What they said ^
    return 0;
}

static const struct flash_parameters *flash_sim_get_parameters(const struct device *dev)
{
    // TODO: ...
    return NULL;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

static DEVICE_API(flash, flash_sim_api) = {
    .read = flash_sim_read,
    .write = flash_sim_write,
    .erase = flash_sim_erase,
    .get_parameters = flash_sim_get_parameters,
    .get_size = flash_sim_get_size,
};

#define SIM_FLASH_DEFINE(n)                                                                                            \
                                                                                                                       \
    static flash_sim_data_t flash_sim_data_##n = {.size_bytes = DT_INST_REG_SIZE(n),                                   \
                                                  .file_path = DT_INST_PROP(n, filename)};                             \
    DEVICE_DT_INST_DEFINE(n, &flash_init, NULL, &flash_sim_data_##n, NULL, POST_KERNEL, CONFIG_FLASH_INIT_PRIORITY,    \
                          &flash_sim_api);

DT_INST_FOREACH_STATUS_OKAY(SIM_FLASH_DEFINE)
