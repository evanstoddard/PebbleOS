/*
 * Copyright (C) Ovyl
 */

/**
 * @file main_event_loop.c
 * @author Evan Stoddard
 * @brief
 */

#include "main_event_loop.h"

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_state.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define MAIN_EVENT_LOOP_DEPTH (16U)

LOG_MODULE_REGISTER(main_event_loop);

/*****************************************************************************
 * Variables
 *****************************************************************************/

K_MSGQ_DEFINE(prv_event_queue, sizeof(PebbleEvent), MAIN_EVENT_LOOP_DEPTH, 1);

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static void prv_render_app(void)
{

    Window *window = window_stack_get_top_window(app_state_get_window_stack());

    window_render(window, app_state_get_graphics_context());

    framebuffer_flush(app_state_get_framebuffer());
}

/**
 * @brief Common things that need to happen every event loop iteration
 */
static void prv_event_loop_upkeep(void)
{
    prv_render_app();
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void main_event_loop(void)
{
    static PebbleEvent event = {0};

    while (true)
    {
        int ret = k_msgq_get(&prv_event_queue, &event, K_FOREVER);

        if (ret < 0)
        {
            LOG_ERR("Failed to dequeue event: %d", ret);
        }

        prv_event_loop_upkeep();
    }
}

void main_event_loop_queue_event(const PebbleEvent *event)
{
    int ret = k_msgq_put(&prv_event_queue, event, K_NO_WAIT);

    if (ret < 0)
    {
        LOG_ERR("Failed to queue event: %d", ret);
    }
}
