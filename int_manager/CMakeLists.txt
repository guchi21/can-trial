cmake_minimum_required(VERSION 3.13)

project(int-manager
    VERSION 1.0.0
    DESCRIPTION "Interrupt manager"
    LANGUAGES CXX
)

add_library(int-manager STATIC
    ./src/int_manager.c
)

target_compile_features(int-manager PRIVATE
    cxx_std_11
)

target_include_directories(int-manager PRIVATE
    ./include
)

set_target_properties(int-manager PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_link_libraries(int-manager PRIVATE
    pico_stdlib
)
