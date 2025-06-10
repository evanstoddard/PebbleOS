/*
 * Copyright (C) Ovyl
 */

/**
 * @file launcher_app.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef launcher_app_h
#define launcher_app_h

#include "ui/window.h"

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

void launcher_app_main(void);

Window *launcher_app_window(void);

void update_progress(uint8_t progress);

#ifdef __cplusplus
}
#endif
#endif /* launcher_app_h */
