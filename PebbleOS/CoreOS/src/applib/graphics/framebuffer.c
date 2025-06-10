/*
 * Copyright (C) Ovyl
 */

/**
 * @file framebuffer.c
 * @author Evan Stoddard
 * @brief Implementation of framebuffer
 */

#include "framebuffer.h"

#include <string.h>
#include <zephyr/drivers/gpio.h>
/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Some define
 */
#define MY_DEFINE (12U)

/**
 * @brief Defines bytes per row
 *
 */
#define FRAMEBUFFER_BYTES_PER_ROW (DEVICE_DISPLAY_WIDTH_PIXELS >> 3)

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void framebuffer_init(FrameBuffer *fb, const GSize *size)
{
    memset(fb, 0, sizeof(FrameBuffer));

    fb->size = *size;
    fb->dirty_rect = GRectZero;
    fb->is_dirty = false;
}

size_t framebuffer_get_size_bytes(FrameBuffer *fb)
{
    return fb->size.h * FRAMEBUFFER_BYTES_PER_ROW;
}

void framebuffer_clear(FrameBuffer *fb)
{
    memset(fb->buffer, 0, FRAMEBUFFER_SIZE_BYTES);
    framebuffer_dirty_all(fb);
}

void framebuffer_mark_dirty_rect(FrameBuffer *fb, GRect rect)
{
    fb->dirty_rect = grect_union(&fb->dirty_rect, &rect);
    fb->is_dirty = true;
}

void framebuffer_dirty_all(FrameBuffer *fb)
{
    fb->dirty_rect = GRect(0, 0, fb->size.w, fb->size.h);
    fb->is_dirty = true;
}

void framebuffer_reset_dirty(FrameBuffer *fb)
{
    fb->dirty_rect = GRectZero;
    fb->is_dirty = false;
}

bool framebuffer_is_dirty(FrameBuffer *fb)
{
    return fb->is_dirty;
}

GSize framebuffer_get_size(FrameBuffer *fb)
{
    return fb->size;
}
