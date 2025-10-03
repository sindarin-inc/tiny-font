cmake_minimum_required(VERSION 3.20)

project(tinyfont_ttf_sdl_example LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(TINY_FONT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

find_package(SDL2 REQUIRED)

add_executable(ttf_sdl
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFFont.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFFontData.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFCache.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFNotoSansLight.cpp
)

target_include_directories(ttf_sdl PRIVATE
    ${TINY_FONT_ROOT}/src
)

# Vendor FreeType like embedded-app
set(FT_DISABLE_PNG ON CACHE BOOL "Disable PNG support in FreeType" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz support in FreeType" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable Brotli support in FreeType" FORCE)
add_subdirectory(${TINY_FONT_ROOT}/freetype/freetype ${CMAKE_CURRENT_BINARY_DIR}/freetype)

target_link_libraries(ttf_sdl PRIVATE SDL2::SDL2 freetype)

target_compile_definitions(ttf_sdl PRIVATE
    CONFIG_FONT_TTF=1
    CONFIG_FONT_IBMF=0
    CONFIG_TTF_SCREEN_RES_PER_INCH=150
    CONFIG_USE_SPIRAM=0
    CONFIG_PIXEL_RESOLUTION_EIGHT_BIT=1
)
