/*
 * Copyright (C) Ovyl
 */

/**
 * @file launcher_app.c
 * @author Evan Stoddard
 * @brief
 */

#include "launcher_app.h"

#include "fonts/font.h"

#include "ui/menu_layer.h"
#include "ui/progress_layer.h"
#include "ui/text_layer.h"
#include "ui/window.h"

#include "resource/resource_system_font.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static Window prv_window = {0};

/**
 * @brief [TODO:description]
 */
static MenuLayer prv_menu;

/**
 * @brief [TODO:description]
 */
static ProgressLayer prv_progress_layer = {0};

/**
 * @brief [TODO:description]
 */
static TextLayer prv_text_layer = {0};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void launcher_app_main(void)
{
    static char pebble_os[] = "PebbleOS";

    window_init(&prv_window, "Launcher");
    menu_layer_init(&prv_menu, prv_window.layer.frame);

    GRect progress_layer_frame = GRect(9, 80, DEVICE_DISPLAY_WIDTH_PIXELS - 18, 8);
    progress_layer_init(&prv_progress_layer, &progress_layer_frame);
    progress_layer_set_progress(&prv_progress_layer, 50);

    GRect text_layer_frame = GRect(40, 50, 20, 20);
    text_layer_init(&prv_text_layer, &text_layer_frame);
    text_layer_set_font(&prv_text_layer, system_get_system_font(0));
    text_layer_set_text(&prv_text_layer, pebble_os);

    layer_add_child((Layer *)&prv_window, (Layer *)&prv_text_layer);
    layer_add_child((Layer *)&prv_window, (Layer *)&prv_progress_layer);
}

Window *launcher_app_window(void)
{
    return &prv_window;
}

void update_progress(uint8_t progress)
{
    progress_layer_set_progress(&prv_progress_layer, progress);
}
