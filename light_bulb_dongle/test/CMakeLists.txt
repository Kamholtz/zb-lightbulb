# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

project(button_press_handler)
# set(SOURCES main.c)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# generate runner for the test
# set(SOURCES button_press_handler_test.c)
test_runner_generate(/home/carlk/repos/zb-lightbulb/light_bulb_dongle/test/src/button_press_handler_test.c)

cmock_handle(/home/carlk/repos/zb-lightbulb/light_bulb_dongle/src/gpio/gpio.h gpio)

# add module gpio
# target_sources(app PRIVATE ${ZEPHYR_BASE}/include/zephyr/drivers/gpio.c)
# target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/zephyr/drivers)


# add module uut
target_sources(app PRIVATE ../src/button_press_handler/button_press_handler.c)
target_include_directories(app PRIVATE ../src/button_press_handler)

# add test file
target_sources(app PRIVATE ./src/button_press_handler_test.c)
target_include_directories(app PRIVATE .)
# target_include_directories(app PRIVATE ./src)

# Preinclude file to the module under test to redefine IS_ENABLED() macro
# which is used in the module.
set_property(SOURCE ../src/button_press_handler/button_press_handler.c PROPERTY COMPILE_FLAGS
	"")

