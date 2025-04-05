/*
 * Copyright (C) Ovyl
 */

/**
 * @file window.c
 * @author Evan Stoddard
 * @brief
 */

#include "window.h"

#include "device_tintin.h"
#include "ui/layer.h"

#include "pebble_device.h"

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

void window_init(Window *window, const char *debug_name)
{
    GRect frame = GRect(0, 0, DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    layer_init(&window->layer, frame);

    window_set_background_color(window, GColorWhite);
}

Layer *window_get_root_layer(const Window *window)
{
    return (Layer *)&window->layer;
}

void window_set_background_color(Window *window, GColor background_color)
{
    window->background_color = background_color;
}

void window_render(Window *window, GContext *ctx)
{
}
