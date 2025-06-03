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
#include "ui/progress_layer.h"
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
static Window *window = NULL;

K_EVENT_DEFINE(prv_update_event);

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

static void prv_loading_sim_cb(struct k_timer *timer)
{
    static uint8_t progress = 0;

    progress++;

    if (progress == 100)
    {
        k_timer_stop(timer);
    }

    update_progress(progress);

    k_event_post(&prv_update_event, UINT32_MAX);
}

K_TIMER_DEFINE(prv_loading_sim_timer, prv_loading_sim_cb, NULL);

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

    window = launcher_app_window();

    window_render(window, &ctx);

    display_flush_framebuffer(&fb);

    k_timer_start(&prv_loading_sim_timer, K_MSEC(100), K_MSEC(100));

    while (true)
    {
        k_event_wait(&prv_update_event, UINT32_MAX, true, K_FOREVER);
        window_render(window, &ctx);
        display_flush_framebuffer(&fb);
    }

    return 0;
}
