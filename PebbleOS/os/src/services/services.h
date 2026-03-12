/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file services.h
 * @author Evan Stoddard
 * @brief OS Services
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
 * @brief Initialize services that kernel depends on
 */
void services_early_init(void);

/**
 * @brief Initialize OS services
 */
void services_init(void);

#ifdef __cplusplus
}
#endif
#endif /* services_h */
