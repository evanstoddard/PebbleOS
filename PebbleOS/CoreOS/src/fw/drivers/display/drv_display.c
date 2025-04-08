/*
 * Copyright (C) Ovyl
 */

/**
 * @file drv_display.c
 * @author Evan Stoddard
 * @brief
 */

#include "drv_display.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>

#include "pebble_device.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

static const struct device *prv_disp = DEVICE_DT_GET(DT_NODELABEL(display));

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void display_flush_framebuffer(FrameBuffer *fb)
{
    struct display_buffer_descriptor desc = {.buf_size = sizeof(fb->buffer),
                                             .height = fb->dirty_rect.size.h,
                                             .width = DEVICE_DISPLAY_WIDTH_PIXELS,
                                             .pitch = DEVICE_DISPLAY_WIDTH_PIXELS};

    display_blanking_off(prv_disp);
    display_write(prv_disp, 0, fb->dirty_rect.origin.y, &desc, fb->buffer);
}
