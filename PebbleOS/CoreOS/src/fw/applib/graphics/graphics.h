/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file graphics.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef graphics_h
#define graphics_h

#include "gcontext.h"
#include "gtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Draw pixel to buffer contained in context
 *
 * @param ctx Pointer to context
 * @param point GPoint to draw pixel to
 */
void graphics_draw_pixel(GContext *ctx, GPoint point);

/**
 * @brief Draw given rect
 * @param ctx Pointer to context
 * @param rect Rect to draw
 */
void graphics_draw_rect(GContext *ctx, GRect rect);

/**
 * @brief Fill in given rect
 *
 * @param ctx Pointer to context
 * @param rect Rect to fill in
 */
void graphics_fill_rect(GContext *ctx, GRect rect);

#ifdef __cplusplus
}
#endif
#endif /* graphics_h */
