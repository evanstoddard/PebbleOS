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
#include "graphics/text.h"
#include "graphics/text_resources.h"
#include "graphics/types/gcolor.h"
#include "ui/progress_layer.h"
#include "ui/text_layer.h"
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

static TextLayer text_layer = {0};

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

    GRect rect = GRect(0, 0, size.w, size.h);
    text_layer_init(&text_layer, &rect);
    text_layer.font.font = (FontMetaData *)&outfile_bin;
    framebuffer_init(&fb, &size);

    graphics_context_init(&ctx, &fb, GContextInitializationMode_System);

    text_layer_set_background_color(&text_layer, GColorWhite);
    text_layer_set_text_color(&text_layer, GColorBlack);

    text_layer_set_text(&text_layer, "Hello Pebble <3");
    /* text_layer.layer.update_proc((Layer *)&text_layer, &ctx); */
    prv_flush_framebuffer(&fb);

    return 0;
}
