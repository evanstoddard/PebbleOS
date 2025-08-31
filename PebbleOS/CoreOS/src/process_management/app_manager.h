/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_manager.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef app_manager_h
#define app_manager_h

#include "process_metadata.h"
#include <stdbool.h>

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
 * @typedef AppLaunchConfig
 * @brief App launch configuration
 *
 */
typedef struct AppLaunchConfig
{
    const ProcessMetadataBase *md;
    bool restart;    //!< Allows the current app to be restarted
    bool forcefully; //!< Causes the current app to be forcefully closed
} AppLaunchConfig;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

int app_manager_start_app(const AppLaunchConfig *launch_config);

#ifdef __cplusplus
}
#endif
#endif /* app_manager_h */
