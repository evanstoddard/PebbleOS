/*
 * Copyright (C) Ovyl
 */

/**
 * @file app_manager.c
 * @author Evan Stoddard
 * @brief
 */

#include "app_manager.h"

#include <errno.h>

#include <stddef.h>

#include "app_state.h"
#include "kernel/pebble_thread.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define APP_STACK_SIZE_BYTES 1024

#define APP_THREAD_PRIORITY 8

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static struct
{

} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief [TODO:description]
 *
 * @param args [TODO:parameter]
 * @return [TODO:return]
 */
void prv_app_thread(void *args)
{
    const AppLaunchConfig *launch_config = (const AppLaunchConfig *)args;

    launch_config->md->main_func();
}

int prv_start_app(const AppLaunchConfig *launch_config)
{
    app_state_init();
    pebble_thread_create_thread(PEBBLE_THREAD_TYPE_APP, APP_STACK_SIZE_BYTES, APP_THREAD_PRIORITY, prv_app_thread,
                                (void *)launch_config);

    return 0;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

int app_manager_start_app(const AppLaunchConfig *launch_config)
{
    if (launch_config == NULL)
    {
        return -EINVAL;
    }

    if (launch_config->md == NULL)
    {
        return -EINVAL;
    }

    prv_start_app(launch_config);
}
