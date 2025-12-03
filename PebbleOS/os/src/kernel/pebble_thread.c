/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_thread.c
 * @author Evan Stoddard
 * @brief
 */

#include "pebble_thread.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pebble_thread);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Wrapper around thread entry
 *
 * @param arg1 Pointer to pebble thread option
 * @param arg2 Unused
 * @param arg3 Unused
 */
static void prv_thread_entry_wrapper(void *thread_inst, void *arg2,
                                     void *arg3) {
  PebbleThread_t *inst = (PebbleThread_t *)thread_inst;

  // NOTE: This will not work on 64-bit systems as zephyr only provides a 32-bit
  // custom data section... whomp whomp...
  k_thread_custom_data_set((void *)inst);

  inst->thread_entry(inst->arg);

  // TODO: Cleanup?
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

PebbleThread_t *pebble_thread_create(const PebbleThreadType_t type,
                                     const char *thread_name,
                                     const size_t stack_size_bytes,
                                     PebbleThread_Func thread_entry,
                                     void *args) {
  if (thread_name == NULL || thread_entry == NULL) {
    return NULL;
  }

  if (type >= PebbleThread_Num) {
    return NULL;
  }

  if (stack_size_bytes == 0) {
    return NULL;
  }

  PebbleThread_t *thread = k_malloc(sizeof(PebbleThread_t));
  if (thread == NULL) {
    LOG_ERR("Failed to allocate thread instance.");
    return NULL;
  }

  thread->stack_buf = k_thread_stack_alloc(stack_size_bytes, 0);

  if (thread->stack_buf == NULL) {
    LOG_ERR("Failed to allocate thread stack.");
    k_free(thread);
    return NULL;
  }

  (void)pebble_thread_init(thread, type, thread_name, thread->stack_buf,
                           stack_size_bytes, thread_entry, args);

  return thread;
}

int pebble_thread_init(PebbleThread_t *thread, const PebbleThreadType_t type,
                       const char *thread_name, void *stack_buf,
                       const size_t stack_size_bytes,
                       PebbleThread_Func thread_entry, void *args) {
  if (thread == NULL || thread_name == NULL || stack_buf == NULL ||
      thread_entry == NULL) {
    LOG_ERR("Pointers passed in are NULL.");
    return 0;
  }

  if (type >= PebbleThread_Num) {
    LOG_ERR("Invalid thread type: 0x%08X", type);
    return -EINVAL;
  }

  if (stack_size_bytes == 0) {
    LOG_ERR("Invalid stack size: %zu", stack_size_bytes);
    return -EINVAL;
  }

  thread->type = type;
  thread->thread_name = thread_name;

  thread->stack_buf = stack_buf;
  thread->stack_size_bytes = stack_size_bytes;

  thread->thread_entry = thread_entry;

  thread->tid = k_thread_create(
      &thread->thread, (k_thread_stack_t *)thread->stack_buf,
      (thread->stack_size_bytes - K_THREAD_STACK_RESERVED),
      prv_thread_entry_wrapper, thread, NULL, NULL, 5, 0, K_NO_WAIT);

  return 0;
}

int pebble_thread_destroy(PebbleThread_t *thread) {
  if (thread == NULL) {
    return -EINVAL;
  }

  // TODO: Thread clean-up

  return 0;
}

PebbleThread_t *pebble_thread_current_thread(void) {
  if (k_is_in_isr()) {
    return NULL;
  }

  return (PebbleThread_t *)k_thread_custom_data_get();
}
