cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(graphics_tests)

# Target Sources
set(CORE_OS_SRC_DIR ${CMAKE_SOURCE_DIR}/../../../../../CoreOS/src)

set(PEBBLE_SOURCES
    ${CORE_OS_SRC_DIR}/fw/applib/graphics/types/grect.c
)

target_include_directories(app PRIVATE
    ${CORE_OS_SRC_DIR}/fw/applib/graphics
)

# Test Sources
target_sources(app PRIVATE 
    src/tests_gpoint.c
    src/tests_gsize.c
    src/tests_grect.c

    ${PEBBLE_SOURCES}
)
