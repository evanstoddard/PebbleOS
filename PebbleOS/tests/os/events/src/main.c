/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file main.c
 * @brief Unit tests for the events module.
 */

#include <zephyr/ztest.h>
#include "events.h"
#include "pebble_thread.h"

/*
 * Stub for pebble_thread_current_thread().
 * In a test context we're not running inside a PebbleThread, so return NULL.
 * events_put_event() handles NULL by routing to the kernel queue.
 */
PebbleThread_t *pebble_thread_current_thread(void) {
	return NULL;
}

/* Re-init event queues before each test */
static void events_before(void *fixture) {
	ARG_UNUSED(fixture);
	events_init();
}

ZTEST_SUITE(events, NULL, NULL, events_before, NULL, NULL);

/* --- events_init --- */

ZTEST(events, test_init_returns_zero) {
	int ret = events_init();
	zassert_equal(ret, 0, "events_init() should return 0");
}

/* --- events_put_event --- */

ZTEST(events, test_put_null_event_returns_einval) {
	int ret = events_put_event(NULL);
	zassert_equal(ret, -EINVAL, "events_put_event(NULL) should return -EINVAL");
}

ZTEST(events, test_put_event_succeeds) {
	PebbleEvent_t ev = {
		.event_type = PEBBLE_BUTTON_DOWN_EVENT,
	};
	int ret = events_put_event(&ev);
	zassert_equal(ret, 0, "events_put_event() should return 0");
}

/* --- events_take --- */

ZTEST(events, test_take_null_event_returns_einval) {
	int ret = events_take(NULL, 0);
	zassert_equal(ret, -EINVAL, "events_take(NULL, ...) should return -EINVAL");
}

ZTEST(events, test_take_timeout_returns_eagain) {
	PebbleEvent_t ev;
	/* No events queued, should timeout immediately */
	int ret = events_take(&ev, 0);
	zassert_equal(ret, -EAGAIN, "events_take() with empty queues should return -EAGAIN");
}

ZTEST(events, test_put_then_take_roundtrip) {
	PebbleEvent_t put_ev = {
		.event_type = PEBBLE_TICK_EVENT,
		.event_bitmask = 0x1234,
	};

	int ret = events_put_event(&put_ev);
	zassert_equal(ret, 0, "put should succeed");

	PebbleEvent_t take_ev = {0};
	ret = events_take(&take_ev, 100);
	zassert_equal(ret, 0, "take should succeed after put");
	zassert_equal(take_ev.event_type, PEBBLE_TICK_EVENT,
		      "event_type should match what was put");
	zassert_equal(take_ev.event_bitmask, 0x1234,
		      "event_bitmask should match what was put");
}

ZTEST(events, test_multiple_events_fifo_order) {
	PebbleEvent_t ev1 = { .event_type = PEBBLE_BUTTON_DOWN_EVENT };
	PebbleEvent_t ev2 = { .event_type = PEBBLE_BUTTON_UP_EVENT };
	PebbleEvent_t ev3 = { .event_type = PEBBLE_TICK_EVENT };

	zassert_equal(events_put_event(&ev1), 0);
	zassert_equal(events_put_event(&ev2), 0);
	zassert_equal(events_put_event(&ev3), 0);

	PebbleEvent_t out = {0};

	zassert_equal(events_take(&out, 100), 0);
	zassert_equal(out.event_type, PEBBLE_BUTTON_DOWN_EVENT, "first event");

	zassert_equal(events_take(&out, 100), 0);
	zassert_equal(out.event_type, PEBBLE_BUTTON_UP_EVENT, "second event");

	zassert_equal(events_take(&out, 100), 0);
	zassert_equal(out.event_type, PEBBLE_TICK_EVENT, "third event");
}

ZTEST(events, test_take_empty_after_drain) {
	PebbleEvent_t ev = { .event_type = PEBBLE_RENDER_REQUEST_EVENT };
	events_put_event(&ev);

	PebbleEvent_t out = {0};
	events_take(&out, 100);

	/* Queue is now empty */
	int ret = events_take(&out, 0);
	zassert_equal(ret, -EAGAIN, "should be empty after draining");
}

ZTEST(events, test_callback_event_roundtrip) {
	static int called = 0;
	called = 0;

	PebbleEvent_t ev = {
		.event_type = PEBBLE_CALLBACK_EVENT,
		.callback = {
			.callback = (PebbleCallbackFunction_t)(void *)0xDEADBEEF,
			.data = (void *)0xCAFEBABE,
		},
	};

	zassert_equal(events_put_event(&ev), 0);

	PebbleEvent_t out = {0};
	zassert_equal(events_take(&out, 100), 0);
	zassert_equal(out.event_type, PEBBLE_CALLBACK_EVENT);
	zassert_equal((uintptr_t)out.callback.callback, 0xDEADBEEF,
		      "callback pointer preserved");
	zassert_equal((uintptr_t)out.callback.data, 0xCAFEBABE,
		      "callback data preserved");
}
