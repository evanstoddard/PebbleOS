/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_heap.h
 * @author Evan Stoddard
 * @brief Global kernel heap from which all kernel resources and other heaps are
 * allocated from
 */

#ifndef kernel_heap_h
#define kernel_heap_h

#include <stddef.h>

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
 * @brief Initialize Kernel Heap
 *
 * @return Return value
 * @retval 0 Success
 */
int kernel_heap_init(void);

/**
 * @brief Allocate RAM from kernel heap.
 *
 * @param size Size to allocate, in bytes.
 * @retval Pointer to allocated memory
 */
void *kernel_heap_malloc(size_t size);

/**
 * @brief Allocate aligned buffer
 *
 * @param align Size to align to
 * @param size Size of allocated memory
 * @retval Pointer to allocated memory
 */
void *kernel_heap_aligned_malloc(size_t align, size_t size);

/**
 * @brief Free allocated memory (Must have been allocated with
 * kernel_heap_* alloc calls)
 *
 * @param ptr Pointer to allocated memory to free
 */
void kernel_heap_free(void *ptr);

#ifdef __cplusplus
}
#endif
#endif /* kernel_heap_h */
