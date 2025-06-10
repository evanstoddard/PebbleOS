/*
 * Copyright (C) Ovyl
 */

/**
 * @file main_event_loop.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef main_event_loop_h
#define main_event_loop_h

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
 * @brief Event loop serviced by main thread
 */
void main_event_loop(void);

#ifdef __cplusplus
}
#endif
#endif /* main_event_loop_h */
