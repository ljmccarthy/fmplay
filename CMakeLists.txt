cmake_minimum_required(VERSION 3.11)
project(fmplay)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Os -fomit-frame-pointer -fwrapv -fstrict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -Wall")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

find_package(SDL2 REQUIRED)
include_directories(fmplay ${SDL2_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/include")

file(GLOB fmplay_SRC "src/*.cpp")
add_executable(fmplay ${fmplay_SRC})
target_link_libraries(fmplay SDL2 SDL2main)

if(0)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
