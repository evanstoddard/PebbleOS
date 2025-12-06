/*
 * Copyright (C) Ovyl
 */

/**
 * @file kvs_iterator.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef kvs_iterator_h
#define kvs_iterator_h

#include "kvs_types.h"

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
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param file [TODO:parameter]
 */
int kvs_iterator_init(KVS_Iterator_t *iterator, pfs_file_t *file);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 */
int kvs_iterator_reset(KVS_Iterator_t *iterator);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_fetch_file_header(KVS_Iterator_t *iterator);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_next_record(KVS_Iterator_t *iterator);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_mark_records_overwrite_begin(KVS_Iterator_t *iterator,
                                              void *key, size_t key_len_bytes);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param key [TODO:parameter]
 * @param key_len_bytes [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_mark_records_overwrite_complete(KVS_Iterator_t *iterator,
                                                 void *key,
                                                 size_t key_len_bytes);

#ifdef __cplusplus
}
#endif
#endif /* kvs_iterator_h */
