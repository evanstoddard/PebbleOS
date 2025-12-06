/*
 * Copyright (C) Ovyl
 */

/**
 * @file kvs_file.h
 * @author Evan Stoddard
 * @brief Persistent key/value pair storaged backed by a file.  For the most
 * part, synonymous with the original `settings_file` "service."  See note
 * below.
 */

/* NOTE:
 *
 * This is effectively the settings_file module that was in the original
 * codebase.  However I've renamed it to kvs_file, to better reflect the
 * intention that this module is a low-level abstraction on top of a PFS
 * (LittleFS) file for storing and retrieving K/V pairs.  This is used to store
 * settings, but can also be used for additional persistent K/V data that needs
 * to be stored.
 *
 * This is the absolute, bare-minimum implementation required to get a working
 * PoC.  There are a lot of guards missing and uncaught edge cases.
 *
 * There are several reasons for this:
 *
 * 1.) I (Evan Stoddard), am lazy.
 * 2.) Wanted to get a working PoC out quickly
 * 3.) I think we should investigate replacing with building zephyr options such
 * as FCB, NVS, ZMS, Zephyr Settings, etc.
 *
 * Using this sub-par implementation should help force this investigation to
 * happen.  Worst case scenario, the original implementation is brought back in.
 */

#ifndef kvs_file_h
#define kvs_file_h

#include <stddef.h>

#include "services/normal/filesystem/pfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define KVS_FILE_MINIMUM_USED_SPACE_BYTES (1024U)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @typedef KVS_File_t
 * @brief [TODO:description]
 *
 */
typedef struct KVS_File_t {
  const char *filename;

  pfs_file_t file;

  // Limits
  size_t max_used_space_bytes;
  size_t max_file_size_bytes;

  // Usage
  size_t used_space;
  size_t dead_space_bytes;
} KVS_File_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Check if KVS file exists (Does not check if file is valid, only if
 * file with that name exists).
 *
 * @param filename File name to check for.
 * @return Return status
 * @retval 1 File exists
 * @retval 0 File does not exist
 * @retval Negative Error checking for file
 */
int kvs_file_exists(const char *filename);

/**
 * @brief Attempt to open an existing KVS file.
 *
 * @param filename Filename to open
 * @retval 0 Successfully opened file
 */
int kvs_file_open(KVS_File_t *kvs_file, const char *filename);

/**
 * @brief [TODO:description]
 *
 * @param filename [TODO:parameter]
 * @param max_used_space_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_file_create(KVS_File_t *kvs_file, const char *filename,
                    size_t max_used_space_bytes);

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @param value [TODO:parameter]
 * @param value_length_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_file_set_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, const void *value,
                      const size_t value_length_bytes);

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @param value_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_file_get_value_len(KVS_File_t *kvs_file, const void *key,
                           const size_t key_len_bytes, size_t *value_len_bytes);

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @param dst [TODO:parameter]
 * @param dst_buf [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_file_get_pair(KVS_File_t *kvs_file, const void *key,
                      const size_t key_len_bytes, void *dst, size_t dst_buf);

/**
 * @brief [TODO:description]
 *
 * @param kvs_file [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_file_delete_pair(KVS_File_t *kvs_file, const void *key,
                         const size_t key_len_bytes);

#ifdef __cplusplus
}
#endif
#endif /* kvs_file_h */
