/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_background.h
 * @author Evan Stoddard
 * @brief Thread handling background kernel task
 */

#ifndef kernel_background_h
#define kernel_background_h

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
 * @brief Initialize background kernel thread
 *
 * @return Returns 0 on success
 */
int kernel_background_init(void);

/**
 * @brief Get pointer to background kernel thread instance
 *
 * @return Pointer to background kernel thread
 */
PebbleThread_t *kernel_background_thread(void);

#ifdef __cplusplus
}
#endif
#endif /* kernel_background_h */
