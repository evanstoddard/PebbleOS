/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file kernel_heap.c
 * @author Evan Stoddard
 * @brief Global kernel heap backed by the RAM region between the end of BSS
 * (_end) and the end of kernel RAM (__kernel_ram_end), as provided by the
 * linker script. All kernel resources and sub-heaps are allocated from here.
 */

#include "kernel_heap.h"

#include <stdint.h>

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(kernel_heap);

/*****************************************************************************
 * Variables
 *****************************************************************************/

// NOTE: Defined by linker script
extern char _end[];
extern char __kernel_ram_end[];

/**
 * @brief Kernel heap object
 */
static struct k_heap prv_heap;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

int kernel_heap_init(void) {
  // NOTE: This is probably a bad idea... but I wasn't sure how much RAM was
  // needed in the Zephyr port, so I'm defining all available heap as the
  // supposed unused amount of RAM and allocation the rest of the OS's heaps
  // based off that... If not explicity defining heap sizes, should at least
  // explicity define a minimum and guard this with an assertion...
  size_t heap_size = (size_t)(__kernel_ram_end - _end);

  LOG_INF("Total available system heap size: %zu bytes", heap_size);

  k_heap_init(&prv_heap, _end, heap_size);

  return 0;
}

void *kernel_heap_malloc(size_t size) {
  __ASSERT(size, "Size must not be 0.");

  void *ptr = k_heap_alloc(&prv_heap, size, K_NO_WAIT);

  return ptr;
}

void *kernel_heap_aligned_malloc(size_t align, size_t size) {
  __ASSERT(size, "Size must not be 0.");

  void *ptr = k_heap_aligned_alloc(&prv_heap, align, size, K_NO_WAIT);

  return ptr;
}

void kernel_heap_free(void *ptr) {
  k_heap_free(&prv_heap, ptr);
}
