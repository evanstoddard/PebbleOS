/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_system_font.c
 * @author Evan Stoddard
 * @brief
 */

#include "resource_system_font.h"

#include "resource_storage.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define NUM_SYSTEM_FONTS 1U

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static FontInfo prv_system_fonts[NUM_SYSTEM_FONTS] = {0};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

GFont system_get_system_font(uint32_t font_id)
{
    if (font_id >= NUM_SYSTEM_FONTS)
    {
        return NULL;
    }

    GFont font = &prv_system_fonts[font_id];

    if (font->loaded == false)
    {
        bool ret = text_resources_init_font(font, SYSTEM_APP, font_id);

        if (ret == false)
        {
            return NULL;
        }
    }

    return font;
}
