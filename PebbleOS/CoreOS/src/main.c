/*
 * Copyright (C) Ovyl
 */

/**
 * @file main.c
 * @author Evan Stoddard
 * @brief
 */

#include <zephyr/kernel.h>

#include "kernel/main_thread.h"

#include "graphics/framebuffer.h"
#include "graphics/gcontext.h"
#include "ui/window.h"

#include "process_management/app_manager.h"

#include "apps/launcher/launcher_app.h"

#include "pebble_device.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

Window window;

GContext context;
FrameBuffer fb;

/*****************************************************************************
 * Functions
 *****************************************************************************/

int main(void)
{

    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    framebuffer_init(&fb, &size);

    graphics_context_init(&context, &fb, GContextInitializationMode_App);

    /* launcher_app_main(); */

    /* window_render(launcher_app_window(), &context); */

    /* framebuffer_flush(&fb); */

    main_thread_init();

    static AppLaunchConfig launch_config = {0};
    launch_config.md = launcher_app_process_metadata();

    app_manager_start_app(&launch_config);

    return 0;
}
