/*
 * Copyright (C) Ovyl
 */

/**
 * @file tests_gpoint.c
 * @author Evan Stoddard
 * @brief GRect Tests
 */

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
  
#include "types/grect.h"

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
 * @brief Test GRectZero macro
 * 
 */
ZTEST(grect_tests, test_grect_zero)
{
    GRect zero = GRectZero;

    zassume_equal(zero.origin.x, 0, "Origin X is zero.");
    zassume_equal(zero.origin.y, 0, "Origin Y is zero.");
    zassume_equal(zero.size.w, 0, "Size w is zero.");
    zassume_equal(zero.size.h, 0, "Size h is zero.");
}

/**
 * @brief Test GRect macro
 * 
 */
ZTEST(grect_tests, test_grect_macro) {
GRect rect = GRect(0xFE, 0xED, 0xBE, 0xEF);

    zassume_equal(rect.origin.x, 0xFE, "Origin X is 0xFE.");
    zassume_equal(rect.origin.y, 0xED, "Origin Y is 0xED.");
    zassume_equal(rect.size.w, 0xBE, "Size w is 0xBE.");
    zassume_equal(rect.size.h, 0xEF, "Size h is 0xEF.");
}

/**
 * @brief Test permutations of is empty function
 * 
 */
ZTEST(grect_tests, test_grect_empty) {
    GRect rect = GRectZero;
    zassume_true(grect_is_empty(&rect), "GRectZero is empty.");

    rect.size.w = 1;
    zassume_true(grect_is_empty(&rect), "Rect with 0 height & nonzero width is empty.");

    rect.size.h = 1;
    zassume_false(grect_is_empty(&rect), "Rect with non-zero h & w is not empty.");

    rect.size.w = 0;
    zassume_true(grect_is_empty(&rect), "Rect with 0 width & nonzero height is empty.");
}

/**
 * @brief Construct a new ZTEST object
 * 
 */
ZTEST(grect_tests, test_grect_union) {
    GRect expected_out = GRectZero;
    GRect out;

    GRect r1 = GRectZero;
    GRect r2 = GRectZero;

    // Both zero
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union is 0.");

    // Return r1
    r1 = GRect(1, 2, 3, 4);
    expected_out = r1;
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union is r1.");

    // Return r2
    r1 = GRectZero;
    r2 = GRect(2, 3, 4, 5);
    expected_out = r2;
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union is r2.");

    // r2 inside r1
    r1 = GRect(10, 10, 10, 10);
    r2 = GRect(12, 12, 5, 5);
    expected_out = r1;
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union is r1. R2 inside r1.");

    // r1 inside r2
    r1 = GRect(25, 25, 10, 10);
    r2 = GRect(20, 20, 20, 20);
    expected_out = r2;
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union is r2. R1 inside r2.");

    // Overlapping
    r1 = GRect(15, 15, 20, 20);
    r2 = GRect(20, 20, 20, 20);
    expected_out = GRect(15, 15, 25, 25);
    out = grect_union(&r1, &r2);
    zexpect_mem_equal(&out, &expected_out, sizeof(GRect), "Union origin 15, 15.  Size 25, 25");
}

/**
 * @brief Test permutations of grect_contains_point
 * 
 */
ZTEST(grect_tests, test_grect_contains_point) {
    // GRect(0, 0, 1, 1) does not contain point (1, 1)
    GRect rect = GRect(0, 0, 1, 1);
    GPoint point = GPoint(1, 1);
    zexpect_false(grect_contains_point(&rect, &point), "GRect(0, 0, 1, 1) does not contain GPoint(1, 1)");

    // GRect(0, 0, 1, 1) does not contain point (0, 1)
    rect = GRect(0, 0, 1, 1);
    point = GPoint(0, 1);
    zexpect_false(grect_contains_point(&rect, &point), "GRect(0, 0, 1, 1) does not contain GPoint(0, 1)");

    // GRect(0, 0, 1, 1) does not contain point (1, 0)
    rect = GRect(0, 0, 1, 1);
    point = GPoint(0, 1);
    zexpect_false(grect_contains_point(&rect, &point), "GRect(0, 0, 1, 1) does not contain GPoint(0, 1)");

    // GRect(0, 0, 1, 1) contains point (0, 0)
    rect = GRect(0, 0, 1, 1);
    point = GPoint(0, 0);
    zexpect_true(grect_contains_point(&rect, &point), "GRect(0, 0, 1, 1) contains GPoint(0, 0)");
}

ZTEST_SUITE(grect_tests, NULL, NULL, NULL, NULL, NULL);
  