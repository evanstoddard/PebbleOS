/*
 * Copyright (C) Ovyl
 */

/**
 * @file pebble_task.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef pebble_task_h
#define pebble_task_h

#include <stddef.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef void (*PebbleTask_Func)(void *arg);

typedef enum {
  PebbleTask_KernelMain,
  PebbleTask_KernelBackground,
  PebbleTask_Worker,
  PebbleTask_App,

  PebbleTask_Num,
} PebbleTaskType_t;

/**
 * @typedef PebbleTask_t
 * @brief Pebble Task definition
 *
 */
typedef struct PebbleTask_t {
  PebbleTaskType_t type;

  const char *task_name;

  size_t stack_size_bytes;
  void *stack_buf;
  int priority;

  PebbleTask_Func thread_entry;
  void *arg;

  struct k_thread thread;
  k_tid_t tid;
} PebbleTask_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Create and initialize a Pebble Task.  Will dynamically allocate thread
 * and stack on the heap.
 *
 * @param type Type of task
 * @param task_name Name of task (Must rename in scope)
 * @param stack_size_bytes Size of stack in bytes.
 * @param task_entry Pointer to entry point of task
 * @param args Optional pointer to args passed to function (Must remain in
 * scope)
 * @return Returns pointer to task
 * @retval NULL Unable to create task
 */
PebbleTask_t *pebble_task_create(const PebbleTaskType_t type,
                                 const char *task_name,
                                 const size_t stack_size_bytes,
                                 PebbleTask_Func task_entry, void *args);

/**
 * @brief Initialize task
 *
 * @param task Pointer to task object
 * @param type Type of task
 * @param task_name Name of task (Must remain in scope)
 * @param stack_buf Pointer to stack buffer (Must remain in scope)
 * @param stack_size_bytes Size of stack in bytes
 * @param task_entry Pointer to entry point of task
 * @param args Optional pointer to args passed to function (Must remain in
 * scope)
 * @return Status of task initialization
 * @retval -EINVAL invalid input arguments
 * @retval 0 Successfully initialized task
 */
int pebble_task_init(PebbleTask_t *task, const PebbleTaskType_t type,
                     const char *task_name, void *stack_buf,
                     size_t stack_size_bytes, PebbleTask_Func task_entry,
                     void *args);

/**
 * @brief Destroy allocated task
 *
 * @param task Pointer to task
 * @return Status of task destruction
 * @retval -EINVAL Invalid arguments
 * @retval 0 Successfully destroyed task
 */
int pebble_task_destroy(PebbleTask_t *task);

#ifdef __cplusplus
}
#endif
#endif /* pebble_task_h */
