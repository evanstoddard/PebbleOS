/*
 * Copyright (C) Ovyl
 */

/**
 * @file tests_gpoint.c
 * @author Evan Stoddard
 * @brief GSize Tests
 */

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
 
 #include "types/gsize.h"
 
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
  * @brief Test GSizeZero macro
  * 
  */
 ZTEST(gsize_tests, test_gsize_zero)
 {
     GSize zero = GSizeZero;
 
     zassume_equal(zero.w, 0, "X is zero.");
     zassume_equal(zero.h, 0, "Y is zero.");
 }
 
 /**
  * @brief Test GSize macro
  * 
  */
 ZTEST(gsize_tests, test_gsize_macro)
 {
     GSize size = GSize(42, 1234);
 
     zassume_equal(size.w, 42, "X is 42.");
     zassume_equal(size.h, 1234, "Y is 1234.");
 }
 
 ZTEST_SUITE(gsize_tests, NULL, NULL, NULL, NULL, NULL);
 