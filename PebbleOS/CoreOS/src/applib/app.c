/*
 * Copyright (C) Ovyl
 */

/**
 * @file app.c
 * @author Evan Stoddard
 * @brief
 */

#include "app.h"

#include <zephyr/kernel.h>

#include "ui/window.h"

#include "kernel/main_event_loop.h"

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
 * @brief Returns if provided window is scheduled for rendering
 *
 * @param window Pointer to window
 * @return True if window has render scheduled
 */
static bool prv_window_render_scheduled(Window *window)
{
    if (window != NULL && window->is_render_schedule)
    {
        return true;
    }

    return false;
}

static bool prv_app_render_scheduled(void)
{
    return false;
}

/**
 * @brief Render current app and kick off event to kernel event loop
 */
static void prv_render_app(void)
{
    PebbleEvent event = {.type = PEBBLE_RENDER_READY_EVENT};

    main_event_loop_queue_event(&event);
}

/**
 * @brief Things that must occur every event loop iteration
 */
void prv_event_loop_upkeep(void)
{
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void app_event_loop(void)
{
    prv_render_app();

    while (true)
    {
        k_sleep(K_FOREVER);

        prv_event_loop_upkeep();
    }
}
