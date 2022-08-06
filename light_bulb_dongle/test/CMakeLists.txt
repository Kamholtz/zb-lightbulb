# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

project(button_press_handler)
# set(SOURCES main.c)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# generate runner for the test
test_runner_generate(button_press_handler_test.c)

# add module uut
target_sources(app PRIVATE ../src/button_press_handler/button_press_handler.c)
target_include_directories(app PRIVATE ../src/button_press_handler)

# add test file
target_sources(app PRIVATE ./button_press_handler_test.c)
target_include_directories(app PRIVATE .)
