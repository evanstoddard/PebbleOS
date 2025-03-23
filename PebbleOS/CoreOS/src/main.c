/*
 * Copyright (C) Ovyl
 */

/**
 * @file main.c
 * @author Evan Stoddard
 * @brief
 */

#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include "device_tintin.h"
#include "graphics/framebuffer.h"
#include "graphics/gcontext.h"
#include "graphics/graphics.h"
#include "ui/progress_layer.h"

#include "graphics/text_resources.h"
#include "zephyr/sys/printk.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define DISPLAY_NODE DT_NODELABEL(display)

/*****************************************************************************
 * Variables
 *****************************************************************************/

static FrameBuffer fb = {0};
static GContext ctx = {0};

static const struct device *const dev = DEVICE_DT_GET(DISPLAY_NODE);

static ProgressLayer progress_layer = {0};

extern char outfile_bin[];

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

static void prv_flush_framebuffer(FrameBuffer *fb)
{
    // Turn off blanking
    display_blanking_off(dev);

    struct display_buffer_descriptor desc = {0};
    desc.buf_size = sizeof(fb->buffer);
    /* desc.height = fb->dirty_rect.size.h; */
    desc.height = DEVICE_DISPLAY_HEIGHT_PIXELS;
    desc.pitch = DEVICE_DISPLAY_WIDTH_PIXELS;
    desc.width = DEVICE_DISPLAY_WIDTH_PIXELS;

    /* display_write(dev, 0, fb->dirty_rect.origin.y, &desc, fb->buffer); */
    display_write(dev, 0, 0, &desc, fb->buffer);
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(void)
{
    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    framebuffer_init(&fb, &size);

    graphics_context_init(&ctx, &fb, GContextInitializationMode_System);
    /**/
    /* GRect progress_rect = GRect(1, 50, 142, 10); */
    /* progress_layer_init(&progress_layer, &progress_rect); */
    /**/
    /* for (uint8_t i = 0; i <= 100; i++) */
    /* { */
    /*     progress_layer_set_progress(&progress_layer, i); */
    /*     progress_layer.layer.update_proc((Layer *)&progress_layer, &ctx); */
    /*     prv_flush_framebuffer(&fb); */
    /*     k_msleep(20); */
    /* } */
    /**/
    // return 0;
    // for (uint16_t y = 0; y < DEVICE_DISPLAY_HEIGHT_PIXELS; y++)
    // {
    //     for (uint16_t x = 0; x < DEVICE_DISPLAY_WIDTH_PIXELS; x++)
    //     {
    //         graphics_draw_pixel(&ctx, GPoint(x, y));
    //         prv_flush_framebuffer(&fb);
    //     }
    // }
    // prv_flush_framebuffer(&fb);

    const GlyphData *glyph = text_resources_get_glyph('D', (FontMetaData *)outfile_bin);

    printk("Width: %u\nHeight: %u\nHorz. Adv: %d\nOffset Left: %d\nOffset Top:%d\n", glyph->header.width,
           glyph->header.height, glyph->header.horizontal_advance, glyph->header.offset_left, glyph->header.offset_top);

    uint16_t start_x = 10;
    uint16_t start_y = 10;

    uint8_t bytes_per_row = glyph->header.width / 8;
    bytes_per_row += (bytes_per_row & 0x7) ? 1 : 0;

    for (uint8_t y = 0; y < glyph->header.height; y++)
    {
        for (uint8_t x = 0; x < glyph->header.width; x++)
        {
            size_t data_idx = (y * bytes_per_row);
            data_idx += (x / 8);

            uint8_t bit_shift = 7 - (x & 0x7);

            if (glyph->data[data_idx] & (1 << bit_shift))
            {
                graphics_draw_pixel(&ctx, GPoint(start_x + x, start_y + y));
            }
        }
    }

    prv_flush_framebuffer(&fb);

    return 0;
}
