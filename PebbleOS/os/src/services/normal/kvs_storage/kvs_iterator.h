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

#define KVS_RECORD_FLAG_OVERWRITE_PENDING 0x80

#define KVS_RECORD_FLAG_OVERWRITE_COMPLETE 0x40

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

/**
 * @brief [TODO:description]
 *
 * @param iterator [TODO:parameter]
 * @param record_header [TODO:parameter]
 * @param record_offset [TODO:parameter]
 * @param flags [TODO:parameter]
 * @return [TODO:return]
 */
int kvs_iterator_clear_flags(KVS_Iterator_t *iterator,
                             KVS_Record_Header_t *record_header,
                             off_t record_offset, uint8_t flags);

#ifdef __cplusplus
}
#endif
#endif /* kvs_iterator_h */
