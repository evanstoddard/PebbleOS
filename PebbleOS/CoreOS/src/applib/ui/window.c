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

/**
 * @brief [TODO:description]
 *
 * @param window [TODO:parameter]
 */
void prv_load_window(Window *window)
{
    if (window->is_loaded)
    {
        return;
    }

    if (window->window_handlers.load != NULL)
    {
        window->window_handlers.load(window);
    }

    window->is_loaded = true;
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

void window_set_on_screen(Window *window, bool on_screen, bool call_handlers)
{
    // Check if window already on screen
    if (window->on_screen == on_screen)
    {
        return;
    }

    window->on_screen = on_screen;

    if (on_screen)
    {
        window_schedule_render(window);
    }
    else
    {
        window->is_render_schedule = false;
        window->on_screen = false;
        window->is_click_configured = false;
    }

    // I really hate nesting
    if (call_handlers == false)
    {
        return;
    }

    if (on_screen)
    {
        prv_load_window(window);

        // Verify the window wasn't unloaded during the loading
        if (window->on_screen && window->window_handlers.appear != NULL)
        {
            window->window_handlers.appear(window);
        }
    }
    else if (window->is_loaded && window->window_handlers.disappear != NULL)
    {
        window->window_handlers.disappear(window);
    }
}

void window_schedule_render(Window *window)
{
    window->is_render_schedule = true;
}
