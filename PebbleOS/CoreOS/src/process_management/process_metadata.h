/*
 * Copyright (C) Ovyl
 */

/**
 * @file process_metadata.h
 * @author Evan Stoddard
 * @brief Module for interacting with process metadata
 */

#ifndef process_metadata_h
#define process_metadata_h

#include <stdbool.h>
#include <stdint.h>

#include "util/uuid.h"

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
 * @brief Function pointer typedef for process entry point
 */
typedef void (*ProcessEntry)(void);

/**
 * @brief
 */
typedef enum
{
    ProcessVisibilityShown = 0,
    ProcessVisibilityHidden = 1,
    ProcessVisibilityShownOnCommunication = 2,
    ProcessVisibilityQuickLaunch = 3,
} ProcessVisibility;

/**
 * @brief Enum declaring the type or process
 */
typedef enum
{
    ProcessTypeApp = 0,
    ProcessTypeWatchface = 1,
    ProcessTypeWorker = 2,
} ProcessType;

/**
 * @brief Run level of process
 */
typedef enum ProcessAppRunLevel
{
    ProcessAppRunLevelNormal = 0,
    ProcessAppRunLevelSystem = 1,
    ProcessAppRunLevelCritical = 2
} ProcessAppRunLevel;

/**
 * @brief Storage medium of process
 */
typedef enum
{
    ProcessStorageBuiltin = 0,
    ProcessStorageFlash = 1,
    ProcessStorageResource = 2
} ProcessStorage;

/**
 * @brief SDK type or process
 */
typedef enum
{
    ProcessAppSDKType_System,
    ProcessAppSDKType_Legacy2x,
    ProcessAppSDKType_Legacy3x,
    ProcessAppSDKType_4x
} ProcessAppSDKType;

/**
 * @typedef ProcessMetadataBase
 * @brief Base process type
 *
 */
typedef struct ProcessMetadataBase
{
    Uuid uuid;

    //! The address of the main function of the process. This will be inside the firmware for firmware processes and
    //! will be inside the process's RAM region for 3rd party processes.
    ProcessEntry main_func;

    //! The type of process
    ProcessType process_type;

    // Flags
    ProcessVisibility visibility;

    //! Where is the process stored?
    ProcessStorage process_storage;

    //! Can this process call kernel functionality directly or does it need to go through syscalls?
    bool is_unprivileged;

    //! Allow Javascript applications to access this process
    bool allow_js;

    //! This process has a sister worker process in flash.
    bool has_worker;

    //! Process is allowed to call RockyJS APIs
    bool is_rocky_app;

    //! Bits of the sdk_platform as they were stored in the binary, or 0 if undefined
    uint16_t stored_sdk_platform;
} PebbleProcessMd;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* process_metadata_h */
