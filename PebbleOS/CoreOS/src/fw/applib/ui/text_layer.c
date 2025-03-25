/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_layer.c
 * @author Evan Stoddard
 * @brief Implementation of text layer
 */

#include "text_layer.h"

#include "graphics/graphics.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

void prv_update_proc(Layer *layer, GContext *ctx)
{
    TextLayer *_layer = (TextLayer *)layer;

    if (_layer->background_color != GColorClear)
    {
        ctx->draw_state.fill_color = _layer->background_color;
        graphics_fill_rect(ctx, _layer->layer.bounds);
    }

    ctx->draw_state.text_color = _layer->text_color;
    graphics_draw_text(ctx, _layer->text, _layer->font, _layer->layer.bounds, _layer->overflow_mode,
                       _layer->text_alignment, NULL);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void text_layer_init(TextLayer *layer, GRect *frame)
{
    layer_init(&layer->layer, *frame);

    layer->layer.update_proc = prv_update_proc;
}

void text_layer_set_text(TextLayer *layer, const char *text)
{
    layer->text = text;
}

void text_layer_set_text_color(TextLayer *layer, GColor color)
{
    layer->text_color = color;
}

void text_layer_set_background_color(TextLayer *layer, GColor color)
{
    layer->background_color = color;
}
