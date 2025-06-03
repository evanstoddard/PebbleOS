#-------------------------------------------------------------------------------
# PebbleOS CoreOS
#

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_C_FLAGS "-Og")

project(CoreOS LANGUAGES C)

target_sources(app PRIVATE 
    src/main.c
    src/fw/applib/graphics/framebuffer.c
    src/fw/applib/graphics/types/grect.c   
    src/fw/applib/graphics/gbitmap.c 
    src/fw/applib/graphics/gcontext.c
    src/fw/applib/graphics/graphics.c
    src/fw/applib/ui/layer.c
    src/fw/applib/ui/window.c
    src/fw/applib/ui/progress_layer.c
    src/fw/applib/ui/text_layer.c
    src/fw/applib/graphics/text_resources.c
    src/fw/applib/graphics/text.c
    src/fw/resource/resource_storage.c
    src/fw/resource/resource_storage_flash.c
    src/fw/resource/resource_system_font.c
    src/fw/drivers/display/drv_display.c
    src/fw/applib/ui/scroll_layer.c
    src/fw/applib/ui/menu_layer.c
    src/fw/apps/launcher/launcher_app.c
    src/fw/applib/ui/menu_iterator.c
    src/fw/applib/fonts/font.c
)

target_include_directories(app PRIVATE
     src/device
     src/fw/applib
     src/fw
)
