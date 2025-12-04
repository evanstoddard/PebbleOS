/*
 * Copyright (C) Ovyl
 */

/**
 * @file normal_services.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef normal_services_h
#define normal_services_h

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
 * @brief Initialize normal services that need to be initialized early
 *
 * @return Status of initialization
 */
int normal_services_init_early(void);

/**
 * @brief Initialize normal services that weren't initialized during early
 * initialization
 *
 * @return Status of initialization
 */
int normal_services_init(void);

#ifdef __cplusplus
}
#endif
#endif /* normal_services_h */
