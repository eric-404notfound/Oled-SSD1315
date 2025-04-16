cmake_minimum_required(VERSION 3.13)

set(PICOTOOL_FETCH_FROM_GIT_PATH "/home/dev/picotool/bin")
#set(PICO_NO_PICOTOOL 1)

set(PICO_SDK_PATH "/home/dev/rp2040/pico-sdk")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(oled C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()
file(GLOB_RECURSE SOURCES "source/*.cpp")


add_subdirectory(libs/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI)

add_executable(CNC
        main.cpp
        ${SOURCES}
)

pico_enable_stdio_uart(oled ON)

# pull in common dependencies
target_link_libraries(oled

        pico_stdlib
        hardware_i2c
)

pico_add_extra_outputs(oled)