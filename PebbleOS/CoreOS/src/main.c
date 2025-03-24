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

    graphics_draw_text(&ctx, "Hello Pebble! <3", (GFont *)&outfile_bin, GRect(10, 10, 134, 50), 0, 0, 0);

    prv_flush_framebuffer(&fb);

    return 0;
}
