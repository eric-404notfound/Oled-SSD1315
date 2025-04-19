cmake_minimum_required(VERSION 3.13)

set(PICOTOOL_FETCH_FROM_GIT_PATH "/home/dev/picotool/bin")
#set(PICO_NO_PICOTOOL 1)

set(PICO_SDK_PATH "/home/dev/rp2040/pico-sdk")

set(PICO_BOARD pico)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(main C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()
file(GLOB_RECURSE SOURCES "source/*.cpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

add_executable(main
        source/main.cpp
        ${SOURCES}
)
pico_enable_stdio_usb(main ON)
pico_enable_stdio_uart(main OFF)


# pull in common dependencies
target_link_libraries(main

        pico_stdlib
        hardware_i2c
        hardware_rtc
        hardware_gpio
)

pico_add_extra_outputs(main)