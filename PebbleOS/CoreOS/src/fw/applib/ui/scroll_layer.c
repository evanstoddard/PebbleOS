/*
 * Copyright (C) Ovyl
 */

/**
 * @file scroll_layer.c
 * @author Evan Stoddard
 * @brief
 */

#include "scroll_layer.h"

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

void scroll_layer_init(ScrollLayer *layer, GRect frame)
{
    layer_init((Layer *)layer, frame);
}

void scroll_layer_set_content_size(ScrollLayer *layer, GSize size)
{
    layer->subcontent_layer.bounds.size = size;
}

GSize scroll_layer_get_content_size(ScrollLayer *layer)
{
    return layer->subcontent_layer.bounds.size;
}
