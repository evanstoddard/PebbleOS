/*
 * Copyright (C) Ovyl
 */

/**
 * @file main_thread.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef main_thread_h
#define main_thread_h

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
 * @brief Initialize main (kernel) thread
 */
void main_thread_init(void);

#ifdef __cplusplus
}
#endif
#endif /* main_thread_h */
