/*
 * Copyright (C) Ovyl
 */

/**
 * @file menu_layer_callbacks.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef menu_layer_callbacks_h
#define menu_layer_callbacks_h

#include <stdint.h>

#include "graphics/gcontext.h"
#include "ui/menu_cell.h"

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
 * @class MenuLayer
 * @brief [TODO:description]
 *
 */
struct MenuLayer;

/**
 * @class Layer
 * @brief [TODO:description]
 *
 */
struct Layer;

typedef uint16_t (*menu_layer_num_sections_cb)(struct MenuLayer *menu_layer, void *callback_ctx);

typedef uint16_t (*menu_layer_num_rows_cb)(struct MenuLayer *menu_layer, uint16_t section, void *callback_ctx);

typedef int16_t (*menu_layer_height_for_row_cb)(struct MenuLayer *menu_layer, MenuIndex *cell_index,
                                                void *callback_ctx);

typedef void (*menu_layer_draw_cell)(GContext *ctx, struct Layer *cell_layer, MenuIndex *cell_index,
                                     void *callback_ctx);

/**
 * @typedef MenuLayerCallbacks
 * @brief [TODO:description]
 *
 */
typedef struct MenuLayerCallbacks
{
    menu_layer_num_sections_cb num_sections;
    menu_layer_num_rows_cb num_rows;
    menu_layer_height_for_row_cb row_height;
    menu_layer_draw_cell draw_cell;
} MenuLayerCallbacks;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* menu_layer_callbacks_h */
