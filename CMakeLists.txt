cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(can-trial
    VERSION 1.0.0
    DESCRIPTION "CAN trial"
    LANGUAGES CXX
)

add_subdirectory(int_manager)
add_subdirectory(can_driver)

add_executable(can-trial
    main.c
)

target_compile_features(can-trial PRIVATE
    cxx_std_11
)
target_include_directories(can-trial PRIVATE
    can_driver/include
    int_manager/include
)

set_target_properties(can-trial PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_link_libraries(can-trial PRIVATE
    pico_stdlib
    int-manager
    can-driver
)

pico_sdk_init()

pico_enable_stdio_usb(can-trial 1)

pico_enable_stdio_uart(can-trial 1)

pico_add_extra_outputs(can-trial)
