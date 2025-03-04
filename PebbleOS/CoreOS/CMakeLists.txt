#-------------------------------------------------------------------------------
# PebbleOS CoreOS
#

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(CoreOS LANGUAGES C)

target_sources(app PRIVATE 
    src/main.c
    src/fw/applib/graphics/framebuffer.c
    src/fw/applib/graphics/types/grect.c    
)

target_include_directories(app PRIVATE
    src/device
    src/fw/applib
)