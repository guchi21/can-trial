cmake_minimum_required( VERSION 3.12 )

project( pico_can
    VERSION 1.0.0
    DESCRIPTION "MCP2515 on RP2040"
    LANGUAGES CXX C ASM
)

add_library( pico_can STATIC
    ./src/pico_can.c
    ./wrapper/wrapper.c
)

target_compile_features( pico_can PRIVATE cxx_std_11 )

target_include_directories( pico_can PUBLIC include PRIVATE wrapper )

target_link_libraries( pico_can
    PRIVATE pico_stdlib
    PRIVATE hardware_spi
    PRIVATE can_driver
)
