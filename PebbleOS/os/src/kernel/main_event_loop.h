/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file main_event_loop.h
 * @author Evan Stoddard
 * @brief Main event loop API. Provides an event loop that dequeues PebbleOS
 * events and dispatches them, as well as a helper for posting callback events
 * to be executed on the kernel main thread.
 */

#ifndef main_event_loop_h
#define main_event_loop_h

#include "events.h"

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
 * @brief Entry point for main event loop
 */
void main_event_loop(void);

/**
 * @brief Post a callback event to be executed on the kernel main event loop.
 * The callback is invoked with @p data on the next iteration of the loop.
 *
 * @param callback Function to invoke on the kernel main thread
 * @param data     Opaque pointer passed through to @p callback
 */
void main_event_loop_add_callback_event(PebbleCallbackFunction_t callback, void *data);

#ifdef __cplusplus
}
#endif
#endif /* main_event_loop_h */
