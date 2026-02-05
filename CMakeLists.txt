cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}lib/pico-sdk)
include(lib/pico-sdk/pico_sdk_init.cmake)

project(ipr C CXX ASM)
pico_sdk_init()

add_executable(ipr
    src/main.cpp
)

# Link hardware_pwm and hardware_adc for this robot
target_link_libraries(ipr
    pico_stdlib
    hardware_pwm
)

pico_enable_stdio_usb(ipr 1)
pico_add_extra_outputs(ipr)
