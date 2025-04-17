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
#include "ui/window.h"

#include "apps/launcher/launcher_app.h"

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

    launcher_app_main();

    Window *window = launcher_app_window();

    window_render(window, &ctx);

    display_flush_framebuffer(&fb);

    return 0;
}
