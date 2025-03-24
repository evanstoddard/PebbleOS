/*
 * Copyright (C) Ovyl
 */

/**
 * @file text.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef text_h
#define text_h

#include "fonts/font.h"
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
 * @brief Enum definition for Text Overflow Mode
 *
 */
typedef enum
{
    GTextOverflowModeWordWrap,
    GTextOverflowModeTrailingEllipsis,
    GTextOverflowModeFill
} GTextOverflowMode;

/**
 * @brief Text Alignment
 *
 */
typedef enum
{
    GTextAlignmentLeft,
    GTextAlignmentCenter,
    GTextAlignmentRight,
} GTextAlignment;

/**
 * @brief Text layout
 *
 */
typedef struct
{
    uint32_t hash;
    GRect box;
    GFont font;
    GTextOverflowMode overflow_mode;
    GTextAlignment alignment;
    GSize max_used_size; //<! Max area occupied by text in px
} TextLayout;

/**
 * @brief Alias for TextLayout
 *
 */
typedef TextLayout GTextAttributes;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Draw text
 *
 * @param ctx [TODO:parameter]
 * @param text [TODO:parameter]
 * @param font [TODO:parameter]
 * @param box [TODO:parameter]
 * @param overflow_mode [TODO:parameter]
 * @param alignment [TODO:parameter]
 * @param text_attributes [TODO:parameter]
 */
void graphics_draw_text(GContext *ctx, const char *text, GFont const *font, const GRect box,
                        const GTextOverflowMode overflow_mode, const GTextAlignment alignment,
                        GTextAttributes *text_attributes);

#ifdef __cplusplus
}
#endif
#endif /* text_h */
