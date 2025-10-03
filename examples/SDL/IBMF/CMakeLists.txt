cmake_minimum_required(VERSION 3.20)

project(tinyfont_ibmf_sdl_example LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Locate the tiny-font repo root from this example dir
get_filename_component(TINY_FONT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

message(STATUS "tiny-font root: ${TINY_FONT_ROOT}")

# SDL2
find_package(SDL2 REQUIRED)

add_executable(ibmf_sdl
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFFont.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFFontData.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFFace.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/RLEExtractor.cpp
)

target_include_directories(ibmf_sdl PRIVATE
    ${TINY_FONT_ROOT}/src
)

target_link_libraries(ibmf_sdl PRIVATE SDL2::SDL2)

target_compile_definitions(ibmf_sdl PRIVATE
    CONFIG_TINYFONT_IBMF=1
    CONFIG_TINYFONT_PIXEL_RESOLUTION_ONE_BIT=1
)
