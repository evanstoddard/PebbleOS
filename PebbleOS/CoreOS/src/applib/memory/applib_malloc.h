/*
 * Copyright (C) Ovyl
 */

/**
 * @file applib_malloc.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef applib_malloc_h
#define applib_malloc_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Allocate memory on heap with size of provided type
 *
 * @param Type Type to determine size of
 */
#define applib_type_zalloc(Type) applib_zalloc(sizeof(Type))

/**
 * @brief Allocate and initialize memory to 0 on heap with size of provided type
 *
 * @param Type Type to determine size of
 */
#define applib_type_malloc(Type) applib_malloc(sizeof(Type))

/**
 * @brief Determine size of provided type
 *
 * @param Type Type to determine size of
 */
#define applib_type_size(Type) sizeof(Type)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Allocate memory on heap
 *
 * @param size Size of memory to be allocated
 */
void *applib_malloc(size_t size);

/**
 * @brief Allocate and initialize memory to 0
 *
 * @param size Size of memory to allocate
 */
void *applib_zalloc(size_t size);

/**
 * @brief Free allocated memory
 *
 * @param ptr Pointer to memory to free
 */
void applib_free(void *ptr);

#ifdef __cplusplus
}
#endif
#endif /* applib_malloc_h */
