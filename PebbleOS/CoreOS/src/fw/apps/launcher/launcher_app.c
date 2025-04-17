/*
 * Copyright (C) Ovyl
 */

/**
 * @file launcher_app.c
 * @author Evan Stoddard
 * @brief
 */

#include "launcher_app.h"

#include "ui/menu_layer.h"
#include "ui/window.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief [TODO:description]
 */
static Window window = {0};

/**
 * @brief [TODO:description]
 */
static MenuLayer menu;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void launcher_app_main(void)
{
    window_init(&window, "Launcher");
    menu_layer_init(&menu, window.layer.frame);

    layer_add_child((Layer *)&window, (Layer *)&menu);
}

Window *launcher_app_window(void)
{
    return &window;
}
