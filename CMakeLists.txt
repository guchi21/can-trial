cmake_minimum_required( VERSION 3.12 )

# Pull in SDK. ( must be before project )
include( pico_sdk_import.cmake )

project( can-trial
    VERSION 1.0.0
    DESCRIPTION "CAN communication trial"
    LANGUAGES CXX C ASM
)

# Initialize SDK.
pico_sdk_init()

add_subdirectory( can_driver )

add_executable( can-trial main.c )

target_link_libraries( can-trial
    PRIVATE pico_stdlib
    PRIVATE can-driver
)

target_compile_features( can-trial PRIVATE cxx_std_11 )

pico_enable_stdio_usb( can-trial 1 )
pico_enable_stdio_uart( can-trial 0 )
pico_add_extra_outputs( can-trial )
