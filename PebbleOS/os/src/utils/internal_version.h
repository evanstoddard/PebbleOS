/*
 * Copyright (C) Ovyl
 */

/**
 * @file internal_version.h
 * @author Evan Stoddard
 * @brief Versioning scheme for Applications and SDK.
 */

#ifndef internal_version_h
#define internal_version_h

#include <stdint.h>

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
 * @typedef Version_t
 * @brief Version for Apps and SDK.  Not firmware.
 *
 */
typedef struct Version_t {
  uint8_t major;
  uint8_t minor;
} __attribute__((__packed__)) Version_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* internal_version_h */
