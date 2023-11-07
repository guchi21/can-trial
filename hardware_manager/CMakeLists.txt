cmake_minimum_required( VERSION 3.12 )

project( hardware_manager
    VERSION 1.0.0
    DESCRIPTION "Hardware manager"
    LANGUAGES CXX C ASM
)

add_library( hardware_manager STATIC
    ./src/hardware_manager.c
)

target_compile_features( hardware_manager PRIVATE cxx_std_11 )

target_include_directories( hardware_manager PUBLIC include )

target_link_libraries( hardware_manager
    PRIVATE pico_stdlib
    PRIVATE hardware_spi
)
