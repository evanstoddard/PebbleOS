/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_task.c
 * @author Evan Stoddard
 * @brief
 */

#include "pebble_task.h"

#include <stdbool.h>

#include <zephyr/logging/log.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(pebble_task);

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Wrapper around task entry
 *
 * @param arg1 Pointer to pebble task option
 * @param arg2 Unused
 * @param arg3 Unused
 */
static void prv_task_entry_wrapper(void *task_inst, void *arg2, void *arg3) {
  PebbleTask_t *inst = (PebbleTask_t *)task_inst;

  inst->thread_entry(inst->arg);

  // TODO: Cleanup?
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

PebbleTask_t *pebble_task_create(const PebbleTaskType_t type,
                                 const char *task_name,
                                 const size_t stack_size_bytes,
                                 PebbleTask_Func task_entry, void *args) {
  if (task_name == NULL || task_entry == NULL) {
    return NULL;
  }

  if (type >= PebbleTask_Num) {
    return NULL;
  }

  if (stack_size_bytes == 0) {
    return NULL;
  }

  PebbleTask_t *task = k_malloc(sizeof(PebbleTask_t));
  if (task == NULL) {
    LOG_ERR("Failed to allocate task instance.");
    return NULL;
  }

  task->stack_buf = k_thread_stack_alloc(stack_size_bytes, 0);

  if (task->stack_buf == NULL) {
    LOG_ERR("Failed to allocate task stack.");
    k_free(task);
    return NULL;
  }

  (void)pebble_task_init(task, type, task_name, task->stack_buf,
                         stack_size_bytes, task_entry, args);

  return task;
}

int pebble_task_init(PebbleTask_t *task, const PebbleTaskType_t type,
                     const char *task_name, void *stack_buf,
                     const size_t stack_size_bytes, PebbleTask_Func task_entry,
                     void *args) {
  if (task == NULL || task_name == NULL || stack_buf == NULL ||
      task_entry == NULL) {
    LOG_ERR("Pointers passed in are NULL.");
    return 0;
  }

  if (type >= PebbleTask_Num) {
    LOG_ERR("Invalid task type: 0x%08X", type);
    return -EINVAL;
  }

  if (stack_size_bytes == 0) {
    LOG_ERR("Invalid stack size: %zu", stack_size_bytes);
    return -EINVAL;
  }

  task->type = type;
  task->task_name = task_name;

  task->stack_buf = stack_buf;
  task->stack_size_bytes = stack_size_bytes;

  task->thread_entry = task_entry;

  task->tid = k_thread_create(
      &task->thread, (k_thread_stack_t *)task->stack_buf,
      (task->stack_size_bytes - K_THREAD_STACK_RESERVED),
      prv_task_entry_wrapper, task, NULL, NULL, 5, 0, K_NO_WAIT);

  return 0;
}

int pebble_task_destroy(PebbleTask_t *task) {
  if (task == NULL) {
    return -EINVAL;
  }

  // TODO: Thread clean-up

  return 0;
}
