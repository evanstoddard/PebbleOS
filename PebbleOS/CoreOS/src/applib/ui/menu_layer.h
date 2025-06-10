/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_layer.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef menu_layer_h
#define menu_layer_h

#include "menu_cell.h"
#include "menu_layer_callbacks.h"
#include "scroll_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef MenuLayer
 * @brief [TODO:description]
 *
 */
typedef struct MenuLayer
{
    ScrollLayer scroll_layer;
    struct
    {
        MenuCellSpan cursor;
    } cache;
    MenuLayerCallbacks callbacks;
} MenuLayer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param frame [TODO:parameter]
 */
void menu_layer_init(MenuLayer *layer, GRect frame);

void menu_layer_reload_data(MenuLayer *layer);

#ifdef __cplusplus
}
#endif
#endif /* menu_layer_h */
