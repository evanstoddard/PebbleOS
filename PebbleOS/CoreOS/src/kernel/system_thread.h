/*
 * Copyright (C) Ovyl
 */

/**
 * @file system_thread.h
 * @author Evan Stoddard
 * @brief System thread (Kernel Background)
 */

#ifndef system_thread_h
#define system_thread_h

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
 * @brief Initialize System Thread
 */
void system_thread_init(void);

#ifdef __cplusplus
}
#endif
#endif /* system_thread_h */
