cmake_minimum_required(VERSION 3.13)

project(
    can_driver
    VERSION 1.0.0
    DESCRIPTION "CAN driver"
    LANGUAGES CXX
)

add_library(can_driver STATIC ./src/can_driver.c)

target_compile_features(can_driver PRIVATE cxx_std_11)

target_include_directories(can_driver INTERFACE ./include)

set_target_properties(
    can_driver
    PROPERTIES
    VERSION ${PROJECT_VERSION}
)
