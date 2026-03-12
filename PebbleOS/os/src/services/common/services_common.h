/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file services_common.h
 * @author Evan Stoddard
 * @brief Common OS services
 */

#ifndef services_common_h
#define services_common_h

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
 * @brief Initialize common OS services
 */
void services_common_init(void);

#ifdef __cplusplus
}
#endif
#endif /* services_common_h */
