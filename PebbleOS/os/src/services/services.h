/*
 * Copyright (C) Ovyl
 */

/**
 * @file services.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef services_h
#define services_h

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
 * @brief Initialze services that need to be initialized early in boot
 *
 * @return Return status of initialization
 */
int services_early_init(void);

/**
 * @brief Initialize services except those initialized early in boot
 *
 * @return Return status of initialization
 */
int services_init(void);

#ifdef __cplusplus
}
#endif
#endif /* services_h */
