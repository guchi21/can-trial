cmake_minimum_required( VERSION 3.12 )

project( com_lib
    VERSION 1.0.0
    DESCRIPTION "Common libraries"
    LANGUAGES CXX C ASM
)

add_library( com_lib STATIC )

target_compile_features( com_lib PRIVATE cxx_std_11 )

target_include_directories( com_lib INTERFACE include )

target_link_libraries( com_lib PRIVATE pico_stdlib )
