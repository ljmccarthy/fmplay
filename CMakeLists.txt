cmake_minimum_required(VERSION 3.11)
project(fmplay)

set(CMAKE_CXX_COMPILER "${PROJECT_SOURCE_DIR}/scripts/llvm-clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -fno-strict-overflow -fno-strict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -fomit-frame-pointer -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -D_DEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--strip-all -Wl,--gc-sections")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

find_package(SDL2 REQUIRED)

set(FMPLAY_SRC "${PROJECT_SOURCE_DIR}/src")
set(FMPLAY_INC "${PROJECT_SOURCE_DIR}/include")

file(GLOB FMPLAY_SRCFILES "${FMPLAY_SRC}/*.cpp")
file(GLOB FMPLAY_APP_SRCFILES "${FMPLAY_SRC}/app/fmplay/*.cpp")

add_executable(fmplay ${FMPLAY_APP_SRCFILES} ${FMPLAY_SRCFILES})
target_include_directories(fmplay PRIVATE "${FMPLAY_INC}")
target_link_libraries(fmplay SDL2::SDL2 SDL2::SDL2main)

add_executable(vgm2samples "${FMPLAY_SRC}/tests/vgm2samples.cpp" ${FMPLAY_SRCFILES})
target_include_directories(vgm2samples PRIVATE "${FMPLAY_INC}")

add_executable(test_vgmplayer "${FMPLAY_SRC}/tests/test_vgmplayer.cpp" ${FMPLAY_SRCFILES})
target_include_directories(test_vgmplayer PRIVATE "${FMPLAY_INC}")

if(0)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
