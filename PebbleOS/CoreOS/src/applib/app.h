/*
 * Copyright (C) Ovyl
 */

/**
 * @file app.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef app_h
#define app_h

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
 * @brief Application event loop, blocks until event received
 */
void app_event_loop(void);

#ifdef __cplusplus
}
#endif
#endif /* app_h */
