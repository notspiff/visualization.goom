cmake_minimum_required(VERSION 3.13)

project(visualization.goom)

set(CPP_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
message(STATUS "KODI_INCLUDE_DIR = \"${KODI_INCLUDE_DIR}\".")

find_package(glm REQUIRED)
message(STATUS "GLM_INCLUDE_DIR = \"${GLM_INCLUDE_DIR}\".")

if(APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM)
  find_package(OpenGl REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(${OPENGL_DEFINITIONS})
else()
  find_package(OpenGLES REQUIRED)
  set(DEPLIBS ${OPENGLES_LIBRARIES})
  set(includes ${OPENGLES_INCLUDE_DIR})
  add_definitions(${OPENGLES_DEFINITIONS})
endif()

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMMON_CXX_WARNINGS -Wall -Wextra -Weffc++ -Wsign-conversion -pedantic)
add_compile_options(${CMAKE_COMMON_C_WARNINGS})
add_compile_options(${CMAKE_COMMON_CXX_WARNINGS})

message(STATUS "Main C++ standard = \"${CMAKE_CXX_STANDARD}\".")

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/lib)

find_package(Freetype REQUIRED)
message(STATUS "FREETYPE_INCLUDE_DIRS = \"${FREETYPE_INCLUDE_DIRS}\".")
message(STATUS "FREETYPE_LIBRARIES = \"${FREETYPE_LIBRARIES}\".")

add_subdirectory(lib)

include_directories(${LIBS_DIR}/goom/include
                    ${LIBS_DIR}/goom/goomutils/include
                    ${LIBS_DIR}
                    SYSTEM ${GLM_INCLUDE_DIR}
                    SYSTEM ../../xbmc/xbmc/xbmc/addons/kodi-dev-kit/include
#                    SYSTEM ${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    SYSTEM ${FREETYPE_INCLUDE_DIRS}
                    SYSTEM ${LIBS_DIR}/vivid/include
                    SYSTEM ${LIBS_DIR}/fmt/include)

set(GOOM_SOURCES src/Main.cpp)

set(GOOM_HEADERS src/CircularBuffer.h
                 src/Main.h)

if ($ENV{DO_TESTING})
  set (TESTING ON)
  include(../visualization.goom.testing/goom_testing)
endif()

list(APPEND DEPLIBS goom goomutils vivid ${FREETYPE_LIBRARIES})

message(STATUS "C Compile features = \"${COMMON_C_COMPILE_FEATURES}\".")
message(STATUS "C++ Compile features = \"${COMMON_CXX_COMPILE_FEATURES}\".")
message(STATUS "C warnings = \"${CMAKE_COMMON_C_WARNINGS}\".")
message(STATUS "C++ warnings = \"${CMAKE_COMMON_CXX_WARNINGS}\".")
message(STATUS "C++ standard required = \"${CMAKE_CXX_STANDARD_REQUIRED}\".")
message(STATUS "C++ extensions = \"${CMAKE_CXX_EXTENSIONS}\".")

build_addon(${PROJECT_NAME} GOOM DEPLIBS)

include(CPack)
