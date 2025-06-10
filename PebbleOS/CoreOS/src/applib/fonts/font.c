/*
 * Copyright (C) Ovyl
 */

/**
 * @file font.c
 * @author Evan Stoddard
 * @brief
 */

#include "font.h"

#include <stdbool.h>
#include <stddef.h>

#include "resource/resource_storage.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

GFont font_load_system_font(uint32_t font_id)
{
    ResourceStorageEntry resource_entry = {0};

    // TODO: I have fonts set up at the beginning of the resource table, but ultimately a map needs to be created.
    bool ret = resource_storage_get_resource(SYSTEM_APP, font_id, &resource_entry);

    if (ret == false)
    {
        return NULL;
    }

    return (GFont)resource_entry.data;
}
