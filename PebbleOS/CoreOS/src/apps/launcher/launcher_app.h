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

#include "process_management/process_metadata.h"

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
 * @brief Get pointer to launcher process metadata
 */
const ProcessMetadataBase *launcher_app_process_metadata(void);

#ifdef __cplusplus
}
#endif
#endif /* launcher_app_h */
