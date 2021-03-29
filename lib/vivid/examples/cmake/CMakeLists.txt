cmake_minimum_required( VERSION 3.8 )

project( vivid_example VERSION 1.0.0 LANGUAGES CXX )

add_executable( vivid_example
        main.cpp
)

target_compile_options ( vivid_example
        PRIVATE
        -Wall -Wextra -pedantic -Wno-effc++ -Wno-sign-conversion
        )

target_include_directories( vivid_example
        PRIVATE
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include
        # Need this '..' hack here. Not nice!
        SYSTEM ${GLM_INCLUDE_DIR}/..
)

set_target_properties( vivid_example
        PROPERTIES
        CXX_STANDARD ${CPP_STANDARD}
)

target_link_libraries( vivid_example
    PRIVATE
        libs::vivid
)
