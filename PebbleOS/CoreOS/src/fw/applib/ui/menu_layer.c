/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_layer.c
 * @author Evan Stoddard
 * @brief
 */

#include "menu_layer.h"

#include "menu_iterator.h"

#include "graphics/graphics.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param ctx [TODO:parameter]
 */
static void prv_update_proc(Layer *layer, GContext *ctx)
{
}

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 */
static void prv_update_caches(MenuLayer *layer)
{
    MenuIterator iterator = {0};
    menu_iterator_init(&iterator, layer);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void menu_layer_init(MenuLayer *layer, GRect frame)
{
    scroll_layer_init(&layer->scroll_layer, frame);

    layer->scroll_layer.layer.update_proc = prv_update_proc;
}
