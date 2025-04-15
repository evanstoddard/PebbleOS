/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file layer.h
 * @author Evan Stoddard
 * @brief Definitions for base type for UI elements
 */

#ifndef layer_h
#define layer_h

#include "graphics/gcontext.h"
#include "graphics/gtypes.h"

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
 * @brief Forward definition of base Layer type.
 *
 */
struct Layer;

/**
 * @class Window
 * @brief [TODO:description]
 *
 */
struct Window;

typedef void (*LayerUpdateProc)(struct Layer *layer, GContext *ctx);

/**
 * @typedef Layer
 * @brief Definition of Layer base type
 *
 */
typedef struct Layer
{
    GRect bounds;
    GRect frame;

    struct layer *first_child;
    struct layer *next_sibling;

    struct Window *window;

    LayerUpdateProc update_proc;
} Layer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize layer
 *
 * @param layer Pointer to layer
 * @param frame Frame of layer
 */
void layer_init(Layer *layer, GRect frame);

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @return [TODO:return]
 */
struct Window *layer_get_window(Layer *layer);

/**
 * @brief [TODO:description]
 *
 * @param node [TODO:parameter]
 * @param ctx [TODO:parameter]
 */
void layer_render_tree(Layer *node, GContext *ctx);

#ifdef __cplusplus
}
#endif
#endif /* layer_h */
