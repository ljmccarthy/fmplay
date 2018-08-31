cmake_minimum_required(VERSION 3.11)
project(fmplay)

set(CMAKE_CXX_COMPILER "${PROJECT_SOURCE_DIR}/scripts/llvm-clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -fwrapv -fstrict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -fomit-frame-pointer -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -D_DEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--strip-all -Wl,--gc-sections")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB fmplay_SRC "src/*.cpp")
add_executable(fmplay ${fmplay_SRC})

find_package(SDL2 REQUIRED)
target_include_directories(fmplay PRIVATE "${PROJECT_SOURCE_DIR}/include" )
target_link_libraries(fmplay SDL2::SDL2 SDL2::SDL2main)

if(0)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
