/*
 * Copyright (C) Ovyl
 */

/**
 * @file kernel_background.h
 * @author Evan Stoddard
 * @brief Kernel background thread
 */

#ifndef kernel_background_h
#define kernel_background_h

#include "pebble_task.h"

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
 * @brief Initialize kernel background thread
 *
 * @return Status of kernel background thread initialization
 * @retval 0 Successfully initialized kernel background thread
 */
int kernel_background_init(void);

/**
 * @brief Get pointer to kernel background thread instance
 *
 * @return Pointer to kernel background thread
 * @retval NULL Kernel background thread not initialized
 */
PebbleTask_t *kernel_background_thread(void);

#ifdef __cplusplus
}
#endif
#endif /* kernel_background_h */
