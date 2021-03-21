cmake_minimum_required(VERSION 3.13)

project(goom LANGUAGES CXX)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")
# To test Kodi, run:
#
#  LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libasan.so.6 kodi &

if(WIN32)
    add_definitions(-D_WIN32PC)
endif()
if ($ENV{DO_GOOM_LOGGING})
    ADD_DEFINITIONS(-DDO_GOOM_LOGGING)
    MESSAGE("Added 'DO_GOOM_LOGGING' #define.")
endif()

add_library(${PROJECT_NAME} 
    STATIC
        src/convolve_fx.cpp
        src/draw_methods.cpp
        src/filter_buffers.cpp
        src/filter_control.cpp
        src/filter_zoom_vector.cpp
        src/filters.cpp
        src/flying_stars_fx.cpp
        src/goom_control.cpp
        src/goom_dots_fx.cpp
        src/goom_draw.cpp
        src/goom_graphic.cpp
        src/goom_stats.cpp
        src/goom_visual_fx.cpp
        src/ifs_dancers_fx.cpp
        src/image_displacement.cpp
        src/image_fx.cpp
        src/lines_fx.cpp
        src/sound_info.cpp
        src/stats/filter_stats.cpp
        src/stats/goom_control_stats.cpp
        src/stats/ifs_stats.cpp
        src/stats/line_stats.cpp
        src/stats/sound_stats.cpp
        src/stats/stars_stats.cpp
        src/stats/tentacles_stats.cpp
        src/tentacles_fx.cpp
        src/tentacle_driver.cpp
        src/tentacles.cpp
        src/tentacles_fx.cpp
        src/text_draw.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${LIBS_DIR}/goom/include
        ${LIBS_DIR}/goomutils/include
    PRIVATE
        ${LIBS_DIR}/goom/include/goom
)

target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE
        ${LIBS_DIR}/vivid/include
        ${LIBS_DIR}/fmt/include
        ${LIBS_DIR}/magic_enum/include
        ${FREETYPE_INCLUDE_DIRS}
)

set_target_properties( ${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD ${CPP_STANDARD}
        POSITION_INDEPENDENT_CODE ON
)

message(STATUS "Goom C++ standard = \"${CPP_STANDARD}\".")
