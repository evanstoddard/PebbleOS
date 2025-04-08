/*
 * Copyright (C) Ovyl
 */

/**
 * @file main.c
 * @author Evan Stoddard
 * @brief
 */

#include <zephyr/kernel.h>

#include "drivers/display/drv_display.h"
#include "graphics/framebuffer.h"
#include "graphics/gcontext.h"
#include "graphics/gcontext_types.h"
#include "graphics/text.h"
#include "graphics/text_resources.h"
#include "resource/resource_storage.h"
#include "resource/resource_system_font.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

static FrameBuffer fb = {0};
static GContext ctx = {0};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(void)
{
    resource_storage_init();
    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    framebuffer_init(&fb, &size);
    graphics_context_init(&ctx, &fb, GContextInitializationMode_App);

    ctx.draw_state.text_color = GColorBlack;

    GRect rect = GRect(10, 10, size.w - 10, 30);

    GFont font = system_get_system_font(0);

    graphics_draw_text(&ctx, "Hello Pebble! <3", font, rect, 0, 0, NULL);

    display_flush_framebuffer(&fb);

    return 0;
}
