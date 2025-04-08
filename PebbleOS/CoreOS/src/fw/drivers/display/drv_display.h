/*
 * Copyright (C) Ovyl
 */

/**
 * @file drv_display.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef drv_display_h
#define drv_display_h

#include "graphics/framebuffer.h"

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

void display_flush_framebuffer(FrameBuffer *fb);

#ifdef __cplusplus
}
#endif
#endif /* drv_display_h */
