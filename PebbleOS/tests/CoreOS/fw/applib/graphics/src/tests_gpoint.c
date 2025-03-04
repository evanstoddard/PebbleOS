/*
 * Copyright (C) Ovyl
 */

/**
 * @file tests_gpoint.c
 * @author Evan Stoddard
 * @brief GPoint Tests
 */

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "types/gpoint.h"

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
 * @brief Test GPointZero macro
 * 
 */
ZTEST(gpoint_tests, test_gpoint_zero)
{
    GPoint zero = GPointZero;

    zassume_equal(zero.x, 0, "X is zero.");
    zassume_equal(zero.y, 0, "Y is zero.");
}

/**
 * @brief Test GSize macro
 * 
 */
ZTEST(gpoint_tests, test_gpoint_macro)
{
    GPoint point = GPoint(42, 1234);

    zassume_equal(point.x, 42, "X is 42.");
    zassume_equal(point.y, 1234, "Y is 1234.");
}

ZTEST_SUITE(gpoint_tests, NULL, NULL, NULL, NULL, NULL);
