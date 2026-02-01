cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
include(pico-sdk/pico_sdk_init.cmake)

project(pendulum_cart C CXX ASM)
pico_sdk_init()

add_executable(pendulum_cart
    src/main.cpp
    src/PID_v1.cpp
    src/dwm_pico_AS5600.c
)

target_include_directories(pendulum_cart PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)
# Link hardware_pwm and hardware_adc for this robot
target_link_libraries(pendulum_cart
    pico_stdlib
    hardware_pwm
    hardware_i2c
    pico_multicore
)

pico_enable_stdio_usb(pendulum_cart 1)
pico_add_extra_outputs(pendulum_cart)
