/*
 * Copyright (C) Ovyl
 */

/**
 * @file scroll_layer.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef scroll_layer_h
#define scroll_layer_h

#include "layer.h"

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
 * @typedef ScrollLayer
 * @brief
 *
 */
typedef struct ScrollLayer
{
    Layer layer;
    Layer subcontent_layer;
} ScrollLayer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param frame [TODO:parameter]
 */
void scroll_layer_init(ScrollLayer *layer, GRect frame);

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param point [TODO:parameter]
 */
void scroll_layer_set_content_size(ScrollLayer *layer, GSize size);

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @return [TODO:return]
 */
GSize scroll_layer_get_content_size(ScrollLayer *layer);

#ifdef __cplusplus
}
#endif
#endif /* scroll_layer_h */
