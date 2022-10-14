# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(plant_sensor_app)

include_directories(include)

target_sources(app PRIVATE 
main.cpp
src/buildin-led.cpp
src/status_codes.cpp
src/battery.cpp
src/adc.cpp
src/moisture_sensor.cpp
)
