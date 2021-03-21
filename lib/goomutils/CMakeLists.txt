cmake_minimum_required(VERSION 3.13)

project(goomutils LANGUAGES CXX)

##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")

if(WIN32)
    add_definitions(-D_WIN32PC)
endif()

add_library( ${PROJECT_NAME} 
    STATIC
        src/all_maps.cpp
        src/colormaps.cpp
        src/colorutils.cpp
        src/goomrand.cpp
        src/image_bitmaps.cpp
        src/logging.cpp
        src/mathutils.cpp
        src/random_colormaps.cpp
        src/random_colormaps_manager.cpp
        src/small_image_bitmaps.cpp
        src/strutils.cpp
        src/thread_pool.cpp
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${LIBS_DIR}/goom/include
        ${LIBS_DIR}/goomutils/include
    PRIVATE
        ${LIBS_DIR}/goomutils/include/goomutils
)
target_include_directories( ${PROJECT_NAME}
    SYSTEM PRIVATE
        ${LIBS_DIR}/vivid/include
        ${LIBS_DIR}/fmt/include
        ${LIBS_DIR}/magic_enum/include
)

set_target_properties( ${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD ${CPP_STANDARD}
        POSITION_INDEPENDENT_CODE ON
)

message(STATUS "Goom Utils C++ standard = \"${CPP_STANDARD}\".")
