cmake_minimum_required( VERSION 3.12 )

project( can-driver
    VERSION 1.0.0
    DESCRIPTION "CAN driver"
    LANGUAGES CXX C ASM
)

add_library(can-driver STATIC
    ./src/pico_can.c
    ./src/can_driver.c
)

target_compile_features( can-driver PRIVATE cxx_std_11 )

target_include_directories( can-driver PUBLIC include )

target_link_libraries( can-driver
    PRIVATE pico_stdlib
    PRIVATE hardware_spi
)
