#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project("Zigbee coordinator")

# NORDIC SDK APP START
target_sources(app PRIVATE
  src/main.c
  src/usb_logging.c
  src/button_press_handler/button_press_handler.c
)
# NORDIC SDK APP END
