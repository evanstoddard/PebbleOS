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

#include <zephyr/fs/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct fs_file_t pfs_file_t;

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

/**
 * @brief Open a file
 *
 * @param file [TODO:parameter]
 * @param filename [TODO:parameter]
 * @param flags [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_open(pfs_file_t *file, const char *filename, fs_mode_t flags);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @param src [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
ssize_t pfs_write(pfs_file_t *file, const void *src, size_t len);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @param dst [TODO:parameter]
 * @param len [TODO:parameter]
 * @return [TODO:return]
 */
ssize_t pfs_read(pfs_file_t *file, void *dst, size_t len);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @param offset [TODO:parameter]
 * @param whence [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_seek(pfs_file_t *file, off_t offset, int whence);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @param offset [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_truncate(pfs_file_t *file, off_t offset);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_close(pfs_file_t *file);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_sync(pfs_file_t *file);

/**
 * @brief [TODO:description]
 *
 * @param file [TODO:parameter]
 * @return [TODO:return]
 */
off_t pfs_tell(pfs_file_t *file);

/**
 * @brief [TODO:description]
 *
 * @param filename [TODO:parameter]
 * @return [TODO:return]
 */
int pfs_delete(const char *filename);

#ifdef __cplusplus
}
#endif
#endif /* pfs_h */
