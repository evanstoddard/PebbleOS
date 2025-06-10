/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_layer.c
 * @author Evan Stoddard
 * @brief
 */

#include "text_layer.h"

#include "graphics/text.h"

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
static void prv_update_layer(struct Layer *layer, GContext *ctx)
{
    TextLayer *_layer = (TextLayer *)layer;

    graphics_draw_text(ctx, _layer->text, _layer->font, layer->frame, 0, 0, NULL);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void text_layer_init(TextLayer *text_layer, const GRect *rect)
{
    layer_init(&text_layer->layer, *rect);

    text_layer->layer.update_proc = prv_update_layer;
}

void text_layer_set_text(TextLayer *text_layer, const char *text)
{
    text_layer->text = text;
}

void text_layer_set_font(TextLayer *text_layer, GFont font)
{
    text_layer->font = font;
}
