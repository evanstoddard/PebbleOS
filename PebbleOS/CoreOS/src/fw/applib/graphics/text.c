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

    /*
     * FIXME: Not sure if this is universally true, but I've seen freetype
     * generate bitmaps where the width is < 1 byte add an extra padding byte,
     * and the pitch is 2 bytes.  Pitch is not brought in to this but maybe it needs
     * to be or the script needs to move the padding bytes to the end and condense
     * the bitmap.
     */
    if (bytes_per_row == 1)
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
            }
        }
    }

    cursor->origin.x = start_x + glyph->header.horizontal_advance;

    ctx->draw_state = prev_state;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void graphics_draw_text(GContext *ctx, const char *text, GFont const *font, const GRect box,
                        const GTextOverflowMode overflow_mode, const GTextAlignment alignment,
                        GTextAttributes *text_attributes)
{
    GRect cursor = box;

    GlyphData *glyph = NULL;

    size_t len = strlen(text);

    for (size_t i = 0; i < len; i++)
    {
        glyph = (GlyphData *)text_resources_get_glyph(text[i], font);
        prv_render_glyph(ctx, glyph, &cursor);
    }
}
