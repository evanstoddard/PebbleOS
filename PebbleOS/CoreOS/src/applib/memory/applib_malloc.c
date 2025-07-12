/*
 * Copyright (C) Ovyl
 */

/**
 * @file applib_malloc.c
 * @author Evan Stoddard
 * @brief
 */

#include "applib_malloc.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Allocate memory on heap
 *
 * @param size Size of memory to be allocated
 */
void *applib_malloc(size_t size)
{
    // TODO: Use k_heap and determine which heap to allocate memory from (e.g. app heap or system heap)
    return malloc(size);
}

/**
 * @brief Allocate and initialize memory to 0
 *
 * @param size Size of memory to allocate
 */
void *applib_zalloc(size_t size)
{
    void *ptr = applib_malloc(size);

    if (ptr == NULL)
    {
        return NULL;
    }

    memset(ptr, 0, size);

    return ptr;
}

/**
 * @brief Free allocated memory
 *
 * @param ptr Pointer to memory to free
 */
void applib_free(void *ptr)
{
    free(ptr);
}
