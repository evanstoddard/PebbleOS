/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_layer.h
 * @author Evan Stoddard
 * @brief Layer for displaying text
 */

#ifndef text_layer_h
#define text_layer_h

#include "layer.h"

#include "fonts/font.h"

#include "graphics/gtypes.h"
#include "graphics/text.h"

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
 * @typedef TextLayer
 * @brief Layer to display text
 *
 */
typedef struct TextLayer
{
    Layer layer;
    const char *text;
    GFont font;
    GColor text_color;
    GColor background_color;
    GTextAlignment text_alignment;
    GTextOverflowMode overflow_mode;
} TextLayer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize text layer
 *
 * @param layer Pointer to layer
 * @param frame Pointer to frame bounds
 */
void text_layer_init(TextLayer *layer, GRect *frame);

/**
 * @brief Change text of layer
 *
 * @param layer Pointer to layer
 * @param text Text to set
 */
void text_layer_set_text(TextLayer *layer, const char *text);

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param color [TODO:parameter]
 */
void text_layer_set_text_color(TextLayer *layer, GColor color);

/**
 * @brief [TODO:description]
 *
 * @param layer [TODO:parameter]
 * @param color [TODO:parameter]
 */
void text_layer_set_background_color(TextLayer *layer, GColor color);

#ifdef __cplusplus
}
#endif
#endif /* text_layer_h */
