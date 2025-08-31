/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_state.c
 * @author Evan Stoddard
 * @brief
 */

#include "app_state.h"

#include "graphics/framebuffer.h"
#include "graphics/gcontext.h"

#include "pebble_device.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef AppState
 * @brief App state that is allocated for each app that is started (TODO: Consider making this an opaque type to allow
 * for multiple running app instance)
 *
 */
typedef struct AppState
{
    FrameBuffer framebuffer;
    GContext graphics_context;
    WindowStack window_stack;
} AppState;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Statically allocated app state for current running app
 */
static AppState prv_current_app_state;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void app_state_init(void)
{
    GSize display_size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);
    framebuffer_init(&prv_current_app_state.framebuffer, &display_size);

    graphics_context_init(&prv_current_app_state.graphics_context, &prv_current_app_state.framebuffer,
                          GContextInitializationMode_App);

    window_stack_init(&prv_current_app_state.window_stack);
}

void app_state_deinit(void)
{
}

FrameBuffer *app_state_get_framebuffer(void)
{
    return &prv_current_app_state.framebuffer;
}

GContext *app_state_get_graphics_context(void)
{
    return &prv_current_app_state.graphics_context;
}

WindowStack *app_state_get_window_stack(void)
{
    return &prv_current_app_state.window_stack;
}
