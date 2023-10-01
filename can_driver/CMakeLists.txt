cmake_minimum_required(VERSION 3.13)

project(
    can-driver
    VERSION 1.0.0
    DESCRIPTION "CAN driver"
    LANGUAGES CXX
)

add_library(can-driver STATIC
    ./src/can_driver.c
    )

target_compile_features(can-driver PRIVATE cxx_std_11)

target_include_directories(can-driver PUBLIC ./include)

set_target_properties(
    can-driver
    PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_link_libraries(can-driver
    pico_stdlib
    hardware_spi
)
