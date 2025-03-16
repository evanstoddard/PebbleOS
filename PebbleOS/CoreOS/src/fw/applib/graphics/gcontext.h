/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file gcontext.h
 * @author Evan Stoddard
 * @brief Context for performing graphics operations
 */

#ifndef gcontext_h
#define gcontext_h

#include "framebuffer.h"
#include "gbitmap.h"
#include "gcontext_types.h"
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

/**
 * @brief Graphics context definition
 *
 */
typedef struct GContext
{
    GBitmap bitmap;
    FrameBuffer *parent_framebuffer;
    GDrawState draw_state;
} GContext;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize graphics context
 *
 * @param ctx
 * @param framebuffer
 * @param init_mode
 */
void graphics_context_init(GContext *ctx, FrameBuffer *framebuffer, GContextInitializationMode init_mode);

/**
 * @brief Mark rect of parent framebuffer dirty
 *
 * @param ctx Pointer to context
 * @param rect Rect to mark dirty
 */
void graphics_context_mark_dirty_rect(GContext *ctx, GRect rect);

#ifdef __cplusplus
}
#endif
#endif /* gcontext_h */
