/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_system_font.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef resource_system_font_h
#define resource_system_font_h

#include "graphics/text_resources.h"

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
 * @brief [TODO:description]
 *
 * @param font_id [TODO:parameter]
 * @return [TODO:return]
 */
GFont system_get_system_font(uint32_t font_id);

#ifdef __cplusplus
}
#endif
#endif /* resource_system_font_h */
