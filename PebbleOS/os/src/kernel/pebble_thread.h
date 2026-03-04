/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file pebble_thread.h
 * @author Evan Stoddard
 * @brief PebbleThread abstraction over Zephyr kernel threads. Provides typed
 * thread creation with heap-allocated stacks, and a way to retrieve the
 * PebbleThread_t for the currently executing thread.
 */

#ifndef pebble_thread_h
#define pebble_thread_h

#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define PEBBLE_THREAD_FLAG_ALLOCATED (1 << 0)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef void (*PebbleThread_Func)(void *arg);

typedef enum {
  PebbleThread_KernelMain,
  PebbleThread_KernelBackground,
  PebbleThread_Worker,
  PebbleThread_App,

  PebbleThread_Num,
} PebbleThreadType_t;

/**
 * @typedef PebbleThread_t
 * @brief Pebble Thread definition
 *
 */
typedef struct PebbleThread_t {
  PebbleThreadType_t type;

  const char *thread_name;

  size_t stack_size_bytes;
  void *stack_buf;
  int priority;

  PebbleThread_Func thread_entry;
  void *arg;

  struct k_thread thread;
  k_tid_t tid;

  uint32_t flags;
} PebbleThread_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Create and initialize a Pebble Thread.  Will dynamically allocate
 * thread and stack on the heap.
 *
 * @param type Type of thread
 * @param thread_name Name of thread (Must rename in scope)
 * @param stack_size_bytes Size of stack in bytes.
 * @param priority Priority of thread
 * @param thread_entry Pointer to entry point of thread
 * @param args Optional pointer to args passed to function (Must remain in
 * scope)
 * @return Returns pointer to thread
 * @retval NULL Unable to create thread
 */
PebbleThread_t *pebble_thread_create(const PebbleThreadType_t type, const char *thread_name,
                                     const size_t stack_size_bytes, const int priority,
                                     PebbleThread_Func thread_entry, void *args);

/**
 * @brief Initialize thread
 *
 * @param thread Pointer to thread object
 * @param type Type of thread
 * @param thread_name Name of thread (Must remain in scope)
 * @param stack_buf Pointer to stack buffer (Must remain in scope)
 * @param stack_size_bytes Size of stack in bytes
 * @param priority Priority
 * @param thread_entry Pointer to entry point of thread
 * @param args Optional pointer to args passed to function (Must remain in
 * scope)
 * @return Status of thread initialization
 * @retval -EINVAL invalid input arguments
 * @retval 0 Successfully initialized thread
 */
int pebble_thread_init(PebbleThread_t *thread, const PebbleThreadType_t type,
                       const char *thread_name, void *stack_buf, const size_t stack_size_bytes,
                       const int priority, const uint32_t flags, PebbleThread_Func thread_entry,
                       void *args);

/**
 * @brief Destroy allocated thread
 *
 * @param thread Pointer to thread
 * @return Status of thread destruction
 * @retval -EINVAL Invalid arguments
 * @retval 0 Successfully destroyed thread
 */
int pebble_thread_destroy(PebbleThread_t *thread);

/**
 * @brief Returns pointer to thread
 *
 * @return Returns pointer to currently running thread.  Calling from ISR will
 * return NULL
 */
PebbleThread_t *pebble_thread_current_thread(void);

#ifdef __cplusplus
}
#endif
#endif /* pebble_thread_h */
