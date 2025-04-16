cmake_minimum_required(VERSION 3.13)

set(PICOTOOL_FETCH_FROM_GIT_PATH "/home/dev/picotool/bin")
#set(PICO_NO_PICOTOOL 1)

set(PICO_SDK_PATH "/home/dev/rp2040/pico-sdk")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(CNC C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()
file(GLOB_RECURSE SOURCES "source/*.cpp")


add_subdirectory(libs/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI)

add_executable(CNC
        main.cpp
        ${SOURCES}
)

target_include_directories(CNC PUBLIC
  ${CMAKE_SOURCE_DIR}/source/CNC
  ${CMAKE_SOURCE_DIR}/source/LCD
  ${CMAKE_SOURCE_DIR}/source/SD_Karte
)

pico_enable_stdio_uart(CNC ON)
pico_enable_stdio_usb(CNC OFF)

pico_generate_pio_header(CNC ${CMAKE_CURRENT_LIST_DIR}/pio/pio_stepper.pio)

# pull in common dependencies
target_link_libraries(CNC

        pico_stdlib
        hardware_pio
        hardware_dma
        hardware_i2c
        hardware_spi
        FatFs_SPI

)

pico_add_extra_outputs(CNC)




add_custom_target(upload 
        COMMAND python3 /home/Dev/Python/Flash_tool.py upload ${PROJECT_NAME}.elf
        DEPENDS CNC
)

add_custom_target(flash
        COMMAND python3 /home/Dev/Python/Flash_tool.py flash ${PROJECT_NAME}.elf RP2040
        DEPENDS CNC upload
)

add_custom_target(console
        COMMAND python3 /home/Dev/Python/Flash_tool.py console 115200
)


add_custom_target(flash_console
        COMMAND ${CMAKE_COMMAND} --build . --target flash
        COMMAND ${CMAKE_COMMAND} --build . --target console
)
