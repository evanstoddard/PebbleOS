/*
 * Copyright (C) Ovyl
 */

/**
 * @file events.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef events_h
#define events_h

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Definition of events
 */
typedef enum
{
    PEBBLE_NULL_EVENT = 0,
    PEBBLE_ACCEL_SHAKE_EVENT,
    PEBBLE_ACCEL_DOUBLE_TAP_EVENT,
    PEBBLE_BT_CONNECTION_EVENT,
    PEBBLE_BT_CONNECTION_DEBOUNCED_EVENT,
    PEBBLE_BUTTON_DOWN_EVENT,
    PEBBLE_BUTTON_UP_EVENT,
    //! From kernel to app, ask the app to render itself
    PEBBLE_RENDER_REQUEST_EVENT,
    //! From app to kernel, ask the compositor to render the app
    PEBBLE_RENDER_READY_EVENT,
    //! From kernel to app, notification that render was completed
    PEBBLE_RENDER_FINISHED_EVENT,
    PEBBLE_BATTERY_CONNECTION_EVENT, // TODO: this has a poor name
    PEBBLE_PUT_BYTES_EVENT,
    PEBBLE_BT_PAIRING_EVENT,
    // Emitted when the Pebble mobile app or third party app is (dis)connected
    PEBBLE_COMM_SESSION_EVENT,
    PEBBLE_MEDIA_EVENT,
    PEBBLE_TICK_EVENT,
    PEBBLE_SET_TIME_EVENT,
    PEBBLE_SYS_NOTIFICATION_EVENT,
    PEBBLE_PROCESS_DEINIT_EVENT,
    PEBBLE_PROCESS_KILL_EVENT,
    PEBBLE_PHONE_EVENT,
    PEBBLE_APP_LAUNCH_EVENT,
    PEBBLE_ALARM_CLOCK_EVENT,
    PEBBLE_SYSTEM_MESSAGE_EVENT,
    PEBBLE_FIRMWARE_UPDATE_EVENT,
    PEBBLE_BT_STATE_EVENT,
    PEBBLE_BATTERY_STATE_CHANGE_EVENT,
    PEBBLE_CALLBACK_EVENT,
    PEBBLE_NEW_APP_MESSAGE_EVENT,
    PEBBLE_SUBSCRIPTION_EVENT,
    PEBBLE_APP_WILL_CHANGE_FOCUS_EVENT,
    PEBBLE_APP_DID_CHANGE_FOCUS_EVENT,
    PEBBLE_DO_NOT_DISTURB_EVENT,
    PEBBLE_REMOTE_APP_INFO_EVENT,
    PEBBLE_ECOMPASS_SERVICE_EVENT,
    PEBBLE_COMPASS_DATA_EVENT,
    PEBBLE_PLUGIN_SERVICE_EVENT,
    PEBBLE_WORKER_LAUNCH_EVENT,
    PEBBLE_BLE_SCAN_EVENT,
    PEBBLE_BLE_CONNECTION_EVENT,
    PEBBLE_BLE_GATT_CLIENT_EVENT,
    PEBBLE_BLE_DEVICE_NAME_UPDATED_EVENT,
    PEBBLE_BLE_HRM_SHARING_STATE_UPDATED_EVENT,
    PEBBLE_WAKEUP_EVENT,
    PEBBLE_BLOBDB_EVENT,
    PEBBLE_VOICE_SERVICE_EVENT,
    PEBBLE_DICTATION_EVENT,
    PEBBLE_APP_FETCH_EVENT,
    PEBBLE_APP_FETCH_REQUEST_EVENT,
    PEBBLE_GATHER_DEBUG_INFO_EVENT,
    PEBBLE_REMINDER_EVENT,
    PEBBLE_CALENDAR_EVENT,
    PEBBLE_PANIC_EVENT,
    PEBBLE_SMARTSTRAP_EVENT,
    //! Event sent back to the app to let them know the result of their sent message.
    PEBBLE_APP_OUTBOX_SENT_EVENT,
    //! A request from the app to the outbox service to handle a message.
    //! @note The consuming service must call app_outbox_service_consume_message() to clean up.
    //! In case the event is dropped because the queue is reset, cleanup happens by events.c in
    //! event_queue_cleanup_and_reset().
    PEBBLE_APP_OUTBOX_MSG_EVENT,
    PEBBLE_HEALTH_SERVICE_EVENT,
    PEBBLE_TOUCH_EVENT,
    PEBBLE_CAPABILITIES_CHANGED_EVENT,
    // Emitted when ANCS disconnects or is invalidated
    PEBBLE_ANCS_DISCONNECTED_EVENT,
    PEBBLE_WEATHER_EVENT,
    PEBBLE_HRM_EVENT,
    PEBBLE_UNOBSTRUCTED_AREA_EVENT,
    PEBBLE_APP_GLANCE_EVENT,
    PEBBLE_TIMELINE_PEEK_EVENT,
    PEBBLE_APP_CACHE_EVENT,
    PEBBLE_ACTIVITY_EVENT,
    PEBBLE_WORKOUT_EVENT,

    PEBBLE_NUM_EVENTS
} PebbleEventType;

typedef struct __attribute__((__packed__)) PebbleEvent
{
    PebbleEventType type : 8;
} PebbleEvent;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* events_h */
