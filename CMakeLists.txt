cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(can-trial)

add_subdirectory(./can_driver)

add_executable(can-trial
    main.c
    mcp2515.c
)

target_link_libraries(can-trial
    pico_stdlib
    hardware_spi
    hardware_clocks
    can_driver
)

pico_sdk_init()

pico_enable_stdio_usb(can-trial 1)

pico_enable_stdio_uart(can-trial 1)

pico_add_extra_outputs(can-trial)
