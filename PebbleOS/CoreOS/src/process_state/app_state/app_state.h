/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_state.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef app_state_h
#define app_state_h

#ifdef __cplusplus
extern "C" {
#endif

#include "graphics/framebuffer.h"
#include "graphics/gcontext.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/*
 * TODO: Eventually this function should return an opaque or void pointer so multiple app states can be initialized at
 * once allowing for true app multitasking with higher-end hardware */

/**
 * @brief Initialize app state
 */
void app_state_init(void);

/**
 * @brief Deinitialize app state
 */
void app_state_deinit(void);

/**
 * @brief Get pointer to app's frame buffer
 *
 * @return Pointer to app's frame buffer
 */
FrameBuffer *app_state_get_framebuffer(void);

/**
 * @brief Get pointer to graphics context
 *
 * @return Pointer to app's graphics context
 */
GContext *app_state_get_graphics_context(void);

#ifdef __cplusplus
}
#endif
#endif /* app_state_h */
