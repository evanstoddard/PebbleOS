#-------------------------------------------------------------------------------
# PebbleOS CoreOS
#

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_C_FLAGS "-Og")

project(CoreOS LANGUAGES C)

target_sources(app PRIVATE 
    src/main.c
)

add_subdirectory(src)



