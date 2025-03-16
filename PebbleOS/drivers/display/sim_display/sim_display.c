/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file sim_display.c
 * @author Evan Stoddard
 * @brief Simulator Display Driver (SDL2 Based)
 */

#include <errno.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <zephyr/logging/log.h>

#include <SDL2/SDL.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Device tree compatability string
 *
 */
#define DT_DRV_COMPAT pebble_sim_display

/**
 * @brief Register driver with logging subsystem
 *
 */
LOG_MODULE_REGISTER(pebble_sim_display, CONFIG_DISPLAY_LOG_LEVEL);

/**
 * @brief Macro to get bytes per row
 *
 */
#define BYTES_PER_ROW (config->width >> 3)

/**
 * @brief
 *
 */
#define BUFFER_IDX(x_coord, y_coord) ((y_coord * BYTES_PER_ROW) + (x_coord >> 3))

/**
 * @brief Macro to get number of bits to shift based on x direction
 *
 */
#define PIXEL_SHIFT_VALUE(x_coord) (x_coord & 0x7)

/**
 * @brief Macro to get mask to set pixel
 *
 */
#define SET_PIXEL(x_coord) (1 << PIXEL_SHIFT_VALUE(x_coord))

/**
 * @brief Macro to get clear pixel mask
 *
 */
#define CLEAR_PIXEL(x_coord) (0xFF ^ SET_PIXEL(x_coord))

/*****************************************************************************
 * Typedefs, Structs, Enums
 *****************************************************************************/

/**
 * @brief Definition of simulator display
 *
 */
typedef struct sim_display_t
{
    uint16_t height;
    uint16_t width;
} sim_display_t;

/**
 * @brief Data container for simulator display
 *
 */
typedef struct sim_display_data_t
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    bool blanking;
} sim_display_data_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Driver & API Functions Functions
 *****************************************************************************/

/**
 * @brief Validate the information in the buffer descriptor
 *
 * @param dev Pointer to device instance
 * @param x Starting x position
 * @param y Starting y position
 * @param desc Pointer to descriptor
 * @return true Descriptor valid
 * @return false Descriptor not valid
 */
bool prv_validate_buffer_descriptor(const struct device *dev, uint16_t x, uint16_t y,
                                    const struct display_buffer_descriptor *desc)
{
    const sim_display_t *config = dev->config;

    if (desc->pitch != desc->width)
    {
        LOG_ERR("Descriptor pitch and descriptor width must be the same.");
        return false;
    }

    if (desc->pitch != config->width)
    {
        LOG_ERR("Descriptor pitch must be equal to display width (%u).", config->width);
        return false;
    }

    if ((desc->height + y) > config->height)
    {
        LOG_ERR("Start position/descriptor height is greater than display height.");
        return false;
    }

    if (desc->buf_size < (desc->height * BYTES_PER_ROW))
    {
        LOG_ERR("Buffer size too small.");
        return false;
    }

    return true;
}

/*****************************************************************************
 * Driver & API Functions Functions
 *****************************************************************************/

/**
 * @brief Initialization of display
 *
 * @param dev Pointer to device entry
 * @return int Returns 0 on success
 */
static int sim_display_init(const struct device *dev)
{
    sim_display_data_t *disp_data = dev->data;
    const sim_display_t *config = dev->config;

    int err = SDL_Init(SDL_INIT_VIDEO);

    if (err != 0)
    {
        LOG_ERR("Failed to initialize SDL. (%d)", err);
        return -ENOMEM;
    }

    err =
        SDL_CreateWindowAndRenderer(config->width * 2, config->height * 2, 0, &disp_data->window, &disp_data->renderer);
    if (err != 0)
    {
        LOG_ERR("Failed to create window and renderer. (%d)", err);
        return -ENOMEM;
    }

    SDL_RenderSetLogicalSize(disp_data->renderer, config->width, config->height);
    disp_data->blanking = false;

    return 0;
}

/**
 * @brief Write buffer to display
 *
 * @param dev Pointer to device entry
 * @param x Origin x coordinate
 * @param y Origin y coordinate
 * @param desc Pointer to buffer descriptor
 * @param buf Pointer to buffer
 * @return int Returns 0 on success
 */
static int sim_display_write(const struct device *dev, const uint16_t x, const uint16_t y,
                             const struct display_buffer_descriptor *desc, const void *buf)
{

    if (desc == NULL)
    {
        LOG_ERR("Buffer descriptor pointer must not be null.");
        return -EINVAL;
    }

    if (buf == NULL)
    {
        LOG_ERR("Buffer pointer must not be null.");
        return -EINVAL;
    }

    sim_display_data_t *disp_data = dev->data;
    const sim_display_t *config = dev->config;

    uint8_t *_buf = (uint8_t *)buf;

    if (prv_validate_buffer_descriptor(dev, x, y, desc) == false)
    {
        return -EINVAL;
    }

    for (uint16_t _y = y; _y < (y + desc->height); _y++)
    {
        for (uint16_t _x = x; _x < config->width; _x++)
        {
            uint8_t val = _buf[BUFFER_IDX(_x, _y)];

            if (val & SET_PIXEL(_x))
            {
                SDL_SetRenderDrawColor(disp_data->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            else
            {
                SDL_SetRenderDrawColor(disp_data->renderer, 0x0, 0x0, 0x0, 0xFF);
            }

            SDL_RenderDrawPoint(disp_data->renderer, _x, _y);
        }
    }

    if (disp_data->blanking == false)
    {
        SDL_RenderPresent(disp_data->renderer);
    }

    return 0;
}

/**
 * @brief Turn off blanking to allow drawing
 *
 * @param dev Pointer to device entry
 * @return int Returns 0 on success
 */
static int sim_display_blanking_off(const struct device *dev)
{
    sim_display_data_t *disp_data = dev->data;

    if (disp_data->blanking == true)
    {
        SDL_RenderPresent(disp_data->renderer);
    }

    disp_data->blanking = false;

    return 0;
}

/**
 * @brief Turn on blanking to prevent drawing to screen
 *
 * @param dev Pointer to device entry
 * @return int Returns 0 on success
 */
static int sim_display_blanking_on(const struct device *dev)
{
    sim_display_data_t *disp_data = dev->data;

    disp_data->blanking = true;

    return 0;
}

static int sim_display_set_brightness(const struct device *dev, const uint8_t brightness)
{
    return -ENOTSUP;
}

static int sim_display_set_contrast(const struct device *dev, const uint8_t contrast)
{
    return -ENOTSUP;
}

static void sim_display_get_capabilities(const struct device *dev, struct display_capabilities *capabilities)
{
}

static int sim_display_set_pixel_format(const struct device *dev, const enum display_pixel_format pixel_format)
{
    // Currently not supported.
    return -ENOTSUP;
}

static DEVICE_API(display, sim_display_api) = {
    .blanking_on = sim_display_blanking_on,
    .blanking_off = sim_display_blanking_off,
    .write = sim_display_write,
    .set_brightness = sim_display_set_brightness,
    .set_contrast = sim_display_set_contrast,
    .get_capabilities = sim_display_get_capabilities,
    .set_pixel_format = sim_display_set_pixel_format,
};

#define SIM_DISPLAY_DEFINE(n)                                                                                          \
    static const sim_display_t sim_display_config_##n = {                                                              \
        .height = DT_INST_PROP(n, height),                                                                             \
        .width = DT_INST_PROP(n, width),                                                                               \
    };                                                                                                                 \
                                                                                                                       \
    static sim_display_data_t sim_display_data_##n;                                                                    \
                                                                                                                       \
    DEVICE_DT_INST_DEFINE(n, &sim_display_init, NULL, &sim_display_data_##n, &sim_display_config_##n, POST_KERNEL,     \
                          CONFIG_DISPLAY_INIT_PRIORITY, &sim_display_api);

DT_INST_FOREACH_STATUS_OKAY(SIM_DISPLAY_DEFINE)
