cmake_minimum_required( VERSION 3.12 )

project(
    vivid
    VERSION 1.1.2
    DESCRIPTION "A simple-to-use cpp color library"
    HOMEPAGE_URL "https://github.com/gurki/vivid"
    LANGUAGES CXX
)

#find_package( glm REQUIRED )
#message(STATUS "glm included")

add_library( vivid STATIC
    src/adobe.cpp
    src/color.cpp
    src/colormap.cpp
    src/hex.cpp
    src/hsl.cpp
    src/hsv.cpp
    src/index.cpp
    src/interpolation.cpp
    src/lab.cpp
    src/lch.cpp
    src/lrgb.cpp
    src/name.cpp
    src/rgb.cpp
    src/rgb32.cpp
    src/rgb8.cpp
    src/srgb.cpp
    src/types.cpp
    src/utility.cpp
    src/xyz.cpp
)

add_library(libs::vivid ALIAS ${PROJECT_NAME})

target_compile_options ( vivid
        PRIVATE
            -Wall -Wextra -pedantic -Wno-effc++ -Wno-sign-conversion
        )

target_compile_features( vivid
#    PUBLIC
#        cxx_std_17  #  std::optional, nested namespaces
    PRIVATE
        cxx_auto_type
        cxx_defaulted_functions
        cxx_lambdas
        cxx_range_for
        cxx_trailing_return_types
        cxx_unicode_literals
        cxx_inline_namespaces
)

target_include_directories( vivid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include
        # Need this '..' hack here. Not nice!
        SYSTEM ${GLM_INCLUDE_DIR}/..
)

set_target_properties( vivid
    PROPERTIES
        CXX_STANDARD ${CPP_STANDARD}
        POSITION_INDEPENDENT_CODE ON
)

################################################################################
#  examples and tests
################################################################################

add_subdirectory(examples/cmake)
add_subdirectory(tests)

message(STATUS "Vivid C++ standard = \"${CPP_STANDARD}\".")
