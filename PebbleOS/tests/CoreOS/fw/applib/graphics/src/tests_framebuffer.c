/*
 * Copyright (C) Ovyl
 */

/**
 * @file tests_framebuffer.c
 * @author Evan Stoddard
 * @brief Framebuffer Tests
 */

 #include <zephyr/kernel.h>
 #include <zephyr/ztest.h>
 
 #include "framebuffer.h"
 
/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Tests
 *****************************************************************************/

/**
 * @brief Test Framebuffer Init
 * 
 */
ZTEST(framebuffer_tests, test_framebuffer_init)
{
    FrameBuffer fb;
    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    framebuffer_init(&fb, &size);

    zexpect_false(fb.is_dirty, "Is dirty initialized to false.");
    
    GRect zero_rect = GRectZero;
    zexpect_mem_equal(&fb.dirty_rect, &zero_rect, sizeof(GRect), "Dirty rect is initialized to GRectZero");

    zexpect_mem_equal(&fb.size, &size, sizeof(GSize), "Framebuffer size initialized to display size.");
}

/**
 * @brief Test Framebuffer Get Size Bytes
 * 
 */
ZTEST(framebuffer_tests, test_framebuffer_get_size_bytes)
{
    FrameBuffer fb;
    
    // Display size
    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);
    framebuffer_init(&fb, &size);

    size_t expected_size = size.h * FRAMEBUFFER_BYTES_PER_ROW;
    size_t fetched_size = framebuffer_get_size_bytes(&fb);
    zexpect_equal(fetched_size, expected_size, "Size is display size in bytes.");

    // Subset size
    size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, 30);
    framebuffer_init(&fb, &size);

    expected_size = size.h * FRAMEBUFFER_BYTES_PER_ROW;
    fetched_size = framebuffer_get_size_bytes(&fb);
    zexpect_equal(fetched_size, expected_size, "Size is specified size in bytes.");
}

/**
 * @brief Test Framebuffer Init
 * 
 */
ZTEST(framebuffer_tests, test_framebuffer_clear)
{
    FrameBuffer fb;
    GSize size = GSize(DEVICE_DISPLAY_WIDTH_PIXELS, DEVICE_DISPLAY_HEIGHT_PIXELS);

    framebuffer_init(&fb, &size);

    zexpect_false(fb.is_dirty, "Is dirty initialized to false.");
    
    GRect zero_rect = GRectZero;
    zexpect_mem_equal(&fb.dirty_rect, &zero_rect, sizeof(GRect), "Dirty rect is initialized to GRectZero");

    // Clear framebuffer
    framebuffer_clear(&fb);
    GRect expected_rect = GRect(0, 0, size.w, size.h);
    zexpect_true(fb.is_dirty, "Is dirty is true.");
    zexpect_mem_equal(&fb.dirty_rect, &expected_rect, sizeof(GRect), "Dirty rect is equal to display rect.");
}

ZTEST_SUITE(framebuffer_tests, NULL, NULL, NULL, NULL, NULL);
