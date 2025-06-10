/*
 * Copyright (C) Ovyl
 */

/**
 * @file window.c
 * @author Evan Stoddard
 * @brief
 */

#include "window.h"

#include "graphics/graphics.h"
#include "ui/layer.h"

#include "pebble_device.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param ctx [TODO:parameter]
 */
void prv_root_layer_update_proc(struct Layer *layer, GContext *ctx)
{
    Window *window = layer_get_window(layer);

    GDrawState orig_draw_state = ctx->draw_state;

    ctx->draw_state.fill_color = window->background_color;
    graphics_fill_rect(ctx, layer->bounds);
    ctx->draw_state = orig_draw_state;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void window_init(Window *window, const char *debug_name)
{
    GRect frame = GRect(0, 0, DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    layer_init(&window->layer, frame);
    window->layer.window = window;
    window->layer.update_proc = prv_root_layer_update_proc;

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
    layer_render_tree(&window->layer, ctx);
}
