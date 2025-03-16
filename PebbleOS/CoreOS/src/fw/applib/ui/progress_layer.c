/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file progress_layer.c
 * @author Evan Stoddard
 * @brief
 */

#include "progress_layer.h"

#include "graphics/graphics.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define PROGRESS_LAYER_INNER_RECT_INSET_PIXELS 2

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Function to render layer
 *
 * @param layer Pointer to base layer
 * @param ctx Pointer to graphics context
 */
void prv_update_layer(struct Layer *layer, GContext *ctx)
{
    ProgressLayer *_layer = (ProgressLayer *)layer;

    // Draw background
    ctx->draw_state.fill_color = _layer->background_color;
    graphics_fill_rect(ctx, _layer->layer.frame);

    // Draw outter layer
    ctx->draw_state.stroke_color = _layer->foreground_color;
    ctx->draw_state.fill_color = _layer->foreground_color;
    graphics_draw_rect(ctx, _layer->layer.frame);

    // Draw inner rect
    uint16_t inner_rect_max_width = layer->frame.size.w - (2 * PROGRESS_LAYER_INNER_RECT_INSET_PIXELS);
    uint16_t inner_rect_width = (_layer->progress_percent * inner_rect_max_width) / 100;
    GRect inner_rect = GRect(layer->frame.origin.x + PROGRESS_LAYER_INNER_RECT_INSET_PIXELS,
                             layer->frame.origin.y + PROGRESS_LAYER_INNER_RECT_INSET_PIXELS, inner_rect_width,
                             layer->frame.size.h - (2 * PROGRESS_LAYER_INNER_RECT_INSET_PIXELS));
    graphics_fill_rect(ctx, inner_rect);
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

void progress_layer_init(ProgressLayer *progress_layer, const GRect *frame)
{
    layer_init(&progress_layer->layer, *frame);
    progress_layer->layer.update_proc = prv_update_layer;

    progress_layer->progress_percent = 0;
    progress_layer->background_color = GColorBlack;
    progress_layer->background_color = GColorWhite;
}

void progress_layer_deinit(ProgressLayer *progress_layer)
{
}

void progress_layer_set_foreground_color(ProgressLayer *progress_layer, GColor color)
{
    progress_layer->foreground_color = color;
}

void progress_layer_set_background_color(ProgressLayer *progress_layer, GColor color)
{
    progress_layer->background_color = color;
}

void progress_layer_set_progress(ProgressLayer *progress_layer, unsigned int progress_percent)
{
    progress_layer->progress_percent = progress_percent;
}
