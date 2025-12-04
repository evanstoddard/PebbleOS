/*
 * Copyright (C) Ovyl
 */

/**
 * @file pfs.h
 * @author Evan Stoddard
 * @brief Filesystem for Pebble (NOTE: This is based on LFS and, therefore, not
 * compatible with the original PFS)
 */

#ifndef pfs_h
#define pfs_h

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
 * @brief Initialize Pebble Filesystem
 *
 * @return Return status of PFS initialization
 * @retval -EALREADY PFS has already been initialized
 * @retval 0 Success
 */
int pfs_init(void);

#ifdef __cplusplus
}
#endif
#endif /* pfs_h */
