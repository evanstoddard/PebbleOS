/*
 * Copyright (C) Ovyl
 */

/**
 * @file resource_storage_impl.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef resource_storage_impl_h
#define resource_storage_impl_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define SYSTEM_APP 0x0U

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @class ResourceStorageEntry
 * @brief [TODO:description]
 *
 */
struct ResourceStorageEntry;

/**
 * @typedef ResourceVersion
 * @brief [TODO:description]
 *
 */
typedef struct __attribute__((packed)) ResourceVersion
{
    uint32_t crc32;
    uint32_t timestamp;
} ResourceVersion;

/**
 * @typedef ResourceManifest
 * @brief [TODO:description]
 *
 */
typedef struct __attribute__((packed)) ResourceManifest
{
    uint32_t num_resources;
    ResourceVersion version;
} ResourceManifest;

/**
 * @typedef ResourceTableEntry
 * @brief [TODO:description]
 *
 */
typedef struct __attribute__((packed)) ResourceTableEntry
{
    uint32_t resource_id;
    uint32_t offset;
    uint32_t length;
    uint32_t crc;
} ResourceTableEntry;

/**
 * @typedef ResourceStoreImpl
 * @brief [TODO:description]
 *
 */
typedef struct ResourceStoreImpl
{
    void (*init)(void);
    bool (*store_valid)(uint32_t app_num);
    bool (*get_resource)(uint32_t app_num, uint32_t resource_id, struct ResourceStorageEntry *entry);
    size_t (*read)(struct ResourceStorageEntry *entry, uint32_t offset, void *dst, size_t size);
} ResourceStoreImpl;

/**
 * @typedef ResourceStorageEntry
 * @brief [TODO:description]
 *
 */
typedef struct ResourceStorageEntry
{
    uint32_t id;
    uint32_t offset;
    uint32_t length;
    ResourceStoreImpl *impl;
    const void *data;
} ResourceStorageEntry;

/**
 * @brief Definition of resource store implementation getter
 */
typedef ResourceStoreImpl *(*ResourceStoreImplGetter)(void);

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* resource_storage_impl_h */
