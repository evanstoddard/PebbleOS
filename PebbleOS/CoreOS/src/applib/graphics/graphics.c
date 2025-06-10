/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file graphics.c
 * @author Evan Stoddard
 * @brief
 */

#include "graphics.h"
#include "graphics/framebuffer.h"
#include "graphics/types/gcolor.h"
/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Low-level set pixel in context bitmap
 *
 * @param ctx Pointer to context
 * @param point Point to set
 * @param set Whether pixel is on (true) or off (false)
 */
void prv_set_pixel(GContext *ctx, GPoint point, bool set)
{
    uint16_t idx = (point.y * FRAMEBUFFER_BYTES_PER_ROW) + (point.x >> 0x3);
    uint8_t *buf = ctx->bitmap.addr;

    if (set)
    {
        uint8_t mask = (1 << (point.x & 0x7));
        buf[idx] |= mask;
    }
    else
    {
        uint8_t mask = ~(1 << (point.x & 0x7));
        buf[idx] &= mask;
    }

    graphics_context_mark_dirty_rect(ctx, GRect(point.x, point.y, 1, 1));
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void graphics_draw_pixel(GContext *ctx, GPoint point)
{
    prv_set_pixel(ctx, point, (ctx->draw_state.stroke_color == GColorBlack));
}

void graphics_draw_rect(GContext *ctx, GRect rect)
{
    // Draw top and bottom horizontal lines
    for (uint16_t x = rect.origin.x; x < rect.origin.x + rect.size.w; x++)
    {
        graphics_draw_pixel(ctx, GPoint(x, rect.origin.y));
        graphics_draw_pixel(ctx, GPoint(x, rect.origin.y + rect.size.h - 1));
    }

    // Draw vertical lines
    for (uint16_t y = rect.origin.y; y < rect.origin.y + rect.size.h; y++)
    {
        graphics_draw_pixel(ctx, GPoint(rect.origin.x, y));
        graphics_draw_pixel(ctx, GPoint(rect.origin.x + rect.size.w - 1, y));
    }
}

void graphics_fill_rect(GContext *ctx, GRect rect)
{
    GColor orig_stroke = ctx->draw_state.stroke_color;
    ctx->draw_state.stroke_color = ctx->draw_state.fill_color;

    for (uint16_t y = rect.origin.y; y < rect.origin.y + rect.size.h; y++)
    {
        for (uint16_t x = rect.origin.x; x < rect.origin.x + rect.size.w; x++)
        {
            graphics_draw_pixel(ctx, GPoint(x, y));
        }
    }

    ctx->draw_state.stroke_color = orig_stroke;
}
