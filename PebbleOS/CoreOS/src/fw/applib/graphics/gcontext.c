/*
 * Copyright (C) Ovyl
 */

/**
 * @file gcontext.c
 * @author Evan Stoddard
 * @brief
 */

#include "gcontext.h"
#include <string.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void graphics_context_init(GContext *ctx, FrameBuffer *framebuffer, GContextInitializationMode init_mode)
{
    memset(ctx, 0, sizeof(GContext));
    ctx->bitmap = framebuffer_get_as_bitmap(framebuffer, &framebuffer->size);
    ctx->parent_framebuffer = framebuffer;
    ctx->draw_state.fill_color = GColorBlack;
    ctx->draw_state.stroke_color = GColorBlack;
    ctx->draw_state.text_color = GColorBlack;
}

void graphics_context_mark_dirty_rect(GContext *ctx, GRect rect)
{
    if (ctx->parent_framebuffer == NULL)
    {
        return;
    }

    framebuffer_mark_dirty_rect(ctx->parent_framebuffer, rect);
}
