/*
 * Copyright (C) Ovyl
 */

/**
 * @file events.c
 * @author Evan Stoddard
 * @brief
 */

#include "events.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "pebble_thread.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

LOG_MODULE_REGISTER(events);

#define EVENTS_NUM_POLLED_EVENT_QUEUES (3U)

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Private instance
 */
static struct {
  struct k_msgq kernel_event_queue;
  struct k_msgq from_app_event_queue;
  struct k_msgq from_worker_event_queue;
  struct k_msgq from_kernel_event_queue;

  uint8_t kernel_event_buf[sizeof(PebbleEvent_t) * CONFIG_MAX_KERNEL_EVENTS];
  uint8_t app_event_buf[sizeof(PebbleEvent_t) * CONFIG_MAX_FROM_APP_EVENTS];
  uint8_t worker_event_buf[sizeof(PebbleEvent_t) * CONFIG_MAX_FROM_WORKER_EVENTS];
  uint8_t from_kernel_event_buf[sizeof(PebbleEvent_t) * CONFIG_MAX_FROM_KERNEL_MAIN_EVENTS];

  struct k_poll_event poll_events[EVENTS_NUM_POLLED_EVENT_QUEUES];
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Queue event to specified queue
 *
 * @param queue Queue to write to
 * @param event Pointer to event to add to queue
 */
static void prv_queue_event(struct k_msgq *queue, PebbleEvent_t *event) {
  k_timeout_t timeout = (!k_is_in_isr()) ? K_MSEC(CONFIG_EVENT_QUEUE_PUT_TIMEOUT_MS) : K_NO_WAIT;

  int ret = k_msgq_put(queue, event, timeout);

  if (ret < 0) {
    LOG_ERR("Failed to queue event: %d", ret);

    // TODO: Handle fatal error
    return;
  }
}

/**
 * @brief Reset polling events
 */
inline static void prv_reset_poll_events(void) {
  for (size_t i = 0; i < EVENTS_NUM_POLLED_EVENT_QUEUES; i++) {
    prv_inst.poll_events[i].state = K_POLL_STATE_NOT_READY;
  }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Initialze event queues
 *
 * @return Return status of event queue initialization
 */
int events_init(void) {
  // Initialize event queues
  k_msgq_init(&prv_inst.kernel_event_queue, prv_inst.kernel_event_buf, sizeof(PebbleEvent_t),
              CONFIG_MAX_KERNEL_EVENTS);
  k_msgq_init(&prv_inst.from_app_event_queue, prv_inst.app_event_buf, sizeof(PebbleEvent_t),
              CONFIG_MAX_FROM_APP_EVENTS);
  k_msgq_init(&prv_inst.from_worker_event_queue, prv_inst.worker_event_buf, sizeof(PebbleEvent_t),
              CONFIG_MAX_FROM_WORKER_EVENTS);
  k_msgq_init(&prv_inst.from_kernel_event_queue, prv_inst.from_kernel_event_buf,
              sizeof(PebbleEvent_t), CONFIG_MAX_FROM_KERNEL_MAIN_EVENTS);

  // Initialize event notifiers for polling
  k_poll_event_init(&prv_inst.poll_events[0], K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                    K_POLL_MODE_NOTIFY_ONLY, &prv_inst.kernel_event_queue);
  k_poll_event_init(&prv_inst.poll_events[1], K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                    K_POLL_MODE_NOTIFY_ONLY, &prv_inst.from_app_event_queue);
  k_poll_event_init(&prv_inst.poll_events[2], K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                    K_POLL_MODE_NOTIFY_ONLY, &prv_inst.from_worker_event_queue);

  return 0;
}

int events_put_event(PebbleEvent_t *event) {
  if (event == NULL) {
    return -EINVAL;
  }

  // All events from an ISR get added to kernel queue
  if (k_is_in_isr()) {
    prv_queue_event(&prv_inst.kernel_event_queue, event);
    return 0;
  }

  PebbleThread_t *calling_thread = pebble_thread_current_thread();

  struct k_msgq *queue = NULL;
  // FIXME: Should probably rethink how I architected threads here.  Threads spun up by zephyr
  // (System Work Queue, BLE threads, etc) won't have the associated PebbleThread_t metadata, so
  // this will be null, unless we attach it.  For now, just blindly queuing to kernel queue
  if (calling_thread == NULL) {
    queue = &prv_inst.kernel_event_queue;
  } else {
    switch (calling_thread->type) {
      case PebbleThread_KernelMain:
        queue = &prv_inst.from_kernel_event_queue;
        break;
      case PebbleThread_App:
        queue = &prv_inst.from_app_event_queue;
        break;
      case PebbleThread_Worker:
        queue = &prv_inst.from_worker_event_queue;
        break;
      case PebbleThread_KernelBackground:
        queue = &prv_inst.kernel_event_queue;
        break;
      default:
        // TODO: WTF
        return -EIO;
    }
  }

  if (queue == NULL) {
    // TODO: Double WTF...
    return -EIO;
  }

  prv_queue_event(queue, event);

  return 0;
}

int events_take(PebbleEvent_t *event, uint32_t timeout_ms) {
  if (event == NULL) {
    return -EINVAL;
  }

  // Always flush the "from kernel" event queue to avoid
  // deadlocks
  int ret = k_msgq_get(&prv_inst.from_kernel_event_queue, event, K_NO_WAIT);
  if (ret == 0) {
    return 0;
  }

  // Poll on events
  ret = k_poll(prv_inst.poll_events, ARRAY_SIZE(prv_inst.poll_events), K_MSEC(timeout_ms));

  if (ret != 0) {
    prv_reset_poll_events();
    return -EAGAIN;
  }

  for (size_t i = 0; i < EVENTS_NUM_POLLED_EVENT_QUEUES; i++) {
    if (prv_inst.poll_events[i].state != K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
      continue;
    }

    ret = k_msgq_get(prv_inst.poll_events[i].msgq, event, K_NO_WAIT);
  }

  prv_reset_poll_events();

  return ret;
}
