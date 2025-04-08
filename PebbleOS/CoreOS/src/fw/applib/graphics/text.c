/*
 * Copyright (C) Ovyl
 */

/**
 * @file text.c
 * @author Evan Stoddard
 * @brief
 */

#include "text.h"

#include <string.h>

#include "graphics.h"
#include "text_resources.h"

#include <zephyr/kernel.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

void prv_render_glyph(GContext *ctx, const GlyphData *glyph, GRect *cursor)
{
    GDrawState prev_state = ctx->draw_state;

    ctx->draw_state.stroke_color = ctx->draw_state.text_color;

    uint16_t start_x = cursor->origin.x + glyph->header.offset_left;
    uint16_t start_y = cursor->origin.y + glyph->header.offset_top;

    uint8_t bytes_per_row = glyph->header.width >> 3;
    bytes_per_row += (glyph->header.width & 0x7) ? 1 : 0;

    // The pitch of the bitmap returned from freetype will always be even and rounded up.
    // So, if the width is 6 pixels, then the resulting bitmap with use the first 6 pixels of
    // the first byte and then pad an extra byte, yielding a pitch of 2 bytes.
    if (bytes_per_row % 2)
    {
        bytes_per_row++;
    }

    for (uint8_t y = 0; y < glyph->header.height; y++)
    {
        for (uint8_t x = 0; x < glyph->header.width; x++)
        {
            size_t data_idx = (y * bytes_per_row);
            data_idx += (x / 8);

            uint8_t bit_shift = 7 - (x & 0x7);

            if (glyph->data[data_idx] & (1 << bit_shift))
            {
                graphics_draw_pixel(ctx, GPoint(start_x + x, start_y + y));
                printk("X");
            }
            else
            {
                printk(" ");
            }
        }
        printk("\n");
    }

    cursor->origin.x = start_x + glyph->header.horizontal_advance;

    ctx->draw_state = prev_state;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void graphics_draw_text(GContext *ctx, const char *text, GFont font, const GRect box,
                        const GTextOverflowMode overflow_mode, const GTextAlignment alignment,
                        GTextAttributes *text_attributes)
{
    if (font == NULL)
    {
        return;
    }

    GRect cursor = box;

    GlyphData *glyph = NULL;

    size_t len = strlen(text);

    for (size_t i = 0; i < len; i++)
    {
        glyph = (GlyphData *)text_resources_get_glyph(&ctx->font_cache, font, text[i]);
        prv_render_glyph(ctx, glyph, &cursor);
    }
}
