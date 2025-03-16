# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0
#
# This CMake file is picked by the Zephyr build system because it is defined
# as the module CMake entry point (see zephyr/module.yml).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(PebbleOS)
add_subdirectory(drivers)

if(CONFIG_ZTEST)
  add_subdirectory(tests)
endif()
