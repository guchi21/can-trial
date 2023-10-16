cmake_minimum_required( VERSION 3.12 )

# Pull in SDK. ( must be before project )
include( pico_sdk_import.cmake )

project( can_trial
    VERSION 1.0.0
    DESCRIPTION "CAN communication trial"
    LANGUAGES CXX C ASM
)

# Initialize SDK.
pico_sdk_init()

add_subdirectory( com_lib )
add_subdirectory( can_driver )

add_executable( can_trial main.c )

target_link_libraries( can_trial
    PRIVATE pico_stdlib
    PRIVATE com_lib
    PRIVATE can_driver
)

target_compile_features( can_trial PRIVATE cxx_std_11 )

pico_enable_stdio_usb( can_trial 1 )
pico_enable_stdio_uart( can_trial 0 )
pico_add_extra_outputs( can_trial )
