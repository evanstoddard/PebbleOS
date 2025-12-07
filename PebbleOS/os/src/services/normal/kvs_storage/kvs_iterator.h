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

#include <stdbool.h>

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
 * @typedef KVS_Record_Foreach_Callback_t
 * @brief
 *
 */
typedef struct KVS_Record_Foreach_Callback_t {
  int (*callback)(KVS_Iterator_t *iterator, off_t record_offset,
                  KVS_Record_Header_t *record_header, void *ctx);
  void *ctx;
} KVS_Record_Foreach_Callback_t;

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
 * @param callback [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_foreach_record(KVS_Iterator_t *iterator,
                                KVS_Record_Foreach_Callback_t *callback);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param filter [TODO:parameter]
 * @param callback [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_filtered_foreach_record(
    KVS_Iterator_t *iterator, KVS_Record_Filter_t *filter,
    KVS_Record_Foreach_Callback_t *callback);

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param filter [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_first_occurence(KVS_Iterator_t *iterator,
                                 KVS_Record_Filter_t *filter,
                                 off_t *record_offset,
                                 KVS_Record_Header_t *record_header);

#ifdef __cplusplus
}
#endif
#endif /* kvs_iterator_h */
