/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file gcontext_types.h
 * @author Evan Stoddard
 * @brief Various types used by GContext
 */

#ifndef gcontext_types_h
#define gcontext_types_h

#include "gtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Initialization mode for GContext
 * 
 */
typedef enum {
    GContextInitializationMode_App,
    GContextInitializationMode_System,
} GContextInitializationMode;

/**
 * @brief Draw state of context
 * 
 */
typedef struct GDrawState {
    GColor stroke_color;
    GColor fill_color;
    GColor text_color;
} GDrawState;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* gcontext_types_h */