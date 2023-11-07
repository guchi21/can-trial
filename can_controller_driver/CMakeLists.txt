cmake_minimum_required( VERSION 3.12 )

project( can_controller_driver
    VERSION 1.0.0
    DESCRIPTION "CAN controller driver"
    LANGUAGES CXX C ASM
)

add_library( can_controller_driver STATIC
    ./src/mcp2515.c
    ./src/mcp2515_receive.c
    ./src/mcp2515_send.c
    ./src/mcp2515_irq.c
)

target_compile_features( can_controller_driver PRIVATE cxx_std_11 )

target_include_directories( can_controller_driver PUBLIC include )

target_link_libraries( can_controller_driver
    PRIVATE pico_stdlib
    PRIVATE hardware_manager
)
