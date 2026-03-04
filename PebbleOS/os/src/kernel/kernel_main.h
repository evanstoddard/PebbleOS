/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_main.h
 * @author Evan Stoddard
 * @brief Main kernel thread
 */

#ifndef kernel_main_h
#define kernel_main_h

#include "pebble_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize kernel main thread
 *
 * @return Return value
 * @retval 0 Success
 */
int kernel_main_init(void);

/**
 * @brief Return pointer to kernel main thread instance
 *
 * @return Pointer to kernel main thread instance
 */
PebbleThread_t *kernel_main_thread(void);

#ifdef __cplusplus
}
#endif
#endif /* kernel_main_h */
