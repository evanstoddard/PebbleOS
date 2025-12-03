/*
 * Copyright (C) Ovyl
 */

/**
 * @file kernel_main.h
 * @author Evan Stoddard
 * @brief Kernel main thread
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
 * @return Return status of initialization
 */
int kernel_main_init(void);

/**
 * @brief Return pointer to main kernel thread instance
 *
 * @return Pointer to kernel thread instance
 * @retval NULL Kernel thread not initialized
 */
PebbleThread_t *kernel_main_thread(void);

#ifdef __cplusplus
}
#endif
#endif /* kernel_main_h */
