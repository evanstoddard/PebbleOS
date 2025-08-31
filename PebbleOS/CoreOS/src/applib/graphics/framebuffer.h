/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file framebuffer.h
 * @author Evan Stoddard
 * @brief Framebuffer for display
 */

#ifndef framebuffer_h
#define framebuffer_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "pebble_device.h"

#include "gtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Defines bytes per row
 *
 */
#define FRAMEBUFFER_BYTES_PER_ROW (DEVICE_DISPLAY_WIDTH_PIXELS >> 3)

/**
 * @brief Devices size of framebuffer in bytes
 *
 */
#define FRAMEBUFFER_SIZE_BYTES (FRAMEBUFFER_BYTES_PER_ROW * DEVICE_DISPLAY_HEIGHT_PIXELS)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Definition of framebuffer
 *
 */
typedef struct FrameBuffer
{
    uint8_t buffer[FRAMEBUFFER_SIZE_BYTES];
    GSize size;
    GRect dirty_rect;
    bool is_dirty;
} FrameBuffer;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize framebuffer
 *
 * @param fb Pointer to framebuffer
 * @param size Size of framebuffer
 */
void framebuffer_init(FrameBuffer *fb, const GSize *size);

/**
 * @brief Get the size of framebuffer in bytes
 *
 * @param fb Pointer to framebuffer
 * @return size_t Size of framebuffer in bytes
 */
size_t framebuffer_get_size_bytes(FrameBuffer *fb);

/**
 * @brief Clear the framebuffer and mark as dirty
 *
 * @param fb Pointer to framebuffer
 */
void framebuffer_clear(FrameBuffer *fb);

/**
 * @brief Mark rect area of framebuffer as dirty
 *
 * @param fb Pointer to framebuffer
 * @param rect Rect to mark dirty
 */
void framebuffer_mark_dirty_rect(FrameBuffer *fb, GRect rect);

/**
 * @brief Mark whole framebuffer as dirty
 *
 * @param fb Pointer to framebuffer
 */
void framebuffer_dirty_all(FrameBuffer *fb);

/**
 * @brief Reset dirty status of framebuffer
 *
 * @param fb Pointer to framebuffer
 */
void framebuffer_reset_dirty(FrameBuffer *fb);

/**
 * @brief Returns dirty status of framebuffer
 *
 * @param fb Pointer to framebuffer
 * @return true Framebuffer is dirty
 * @return false Framebuffer is not dirty
 */
bool framebuffer_is_dirty(FrameBuffer *fb);

/**
 * @brief Return GSize of framebuffer
 *
 * @param fb Pointer to framebuffer
 * @return GSize Size of framebuffer
 */
GSize framebuffer_get_size(FrameBuffer *fb);

/**
 * @brief Flush framebuffer to display
 *
 * @param fb Pointer to frame buffer
 */
void framebuffer_flush(FrameBuffer *fb);

#ifdef __cplusplus
}
#endif
#endif /* framebuffer_h */
