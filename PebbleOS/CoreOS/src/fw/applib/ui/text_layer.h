/*
 * Copyright (C) Ovyl
 */

/**
 * @file text_layer.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef text_layer_h
#define text_layer_h

#include "layer.h"

#include "fonts/font.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct TextLayer
{
    Layer layer;
    GFont font;
    const char *text;
} TextLayer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param text_layer [TODO:parameter]
 * @param rect [TODO:parameter]
 */
void text_layer_init(TextLayer *text_layer, const GRect *rect);

/**
 * @brief [TODO:description]
 *
 * @param text_layer [TODO:parameter]
 * @param text [TODO:parameter]
 */
void text_layer_set_text(TextLayer *text_layer, const char *text);

/**
 * @brief [TODO:description]
 *
 * @param text_layer [TODO:parameter]
 * @param font [TODO:parameter]
 */
void text_layer_set_font(TextLayer *text_layer, GFont font);

#ifdef __cplusplus
}
#endif
#endif /* text_layer_h */
