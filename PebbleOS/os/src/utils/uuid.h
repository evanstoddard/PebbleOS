/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file uuid.h
 * @author Evan Stoddard
 * @brief Collection of UUID definitions and helper functions
 */

#ifndef uuid_h
#define uuid_h

#include <stdbool.h>

#include <zephyr/sys/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define UUID_SYSTEM ((struct uuid){0})

#define UUID_INVALID                                                                           \
  ((struct uuid){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                 0xFF, 0xFF, 0xFF})

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Check if UUID is a system UUID
 *
 * @param uuid Pointer to UUID
 * @return True if UUID is system UUID
 */
bool uuid_is_system(struct uuid *uuid);

/**
 * @brief Checks if UUID is invalid
 *
 * @param uuid Pointer to UUID
 * @return True if UUID is invalid
 */
bool uuid_is_invalid(struct uuid *uuid);

#ifdef __cplusplus
}
#endif
#endif /* uuid_h */
