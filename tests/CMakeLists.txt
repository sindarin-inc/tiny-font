cmake_minimum_required(VERSION 3.20)

project(tinyfont_tests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(TINY_FONT_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_library(Catch2 STATIC ${CMAKE_CURRENT_LIST_DIR}/Catch2/catch_amalgamated.cpp)
target_include_directories(Catch2 PUBLIC ${CMAKE_CURRENT_LIST_DIR})

enable_testing()

find_package(PNG REQUIRED)

add_executable(tests_ibmf ${CMAKE_CURRENT_LIST_DIR}/TestIBMF.cpp ${CMAKE_CURRENT_LIST_DIR}/ImageIO.cpp)
target_include_directories(tests_ibmf PRIVATE ${TINY_FONT_ROOT}/src ${TINY_FONT_ROOT}/src/UI/Fonts ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(tests_ibmf PRIVATE CONFIG_FONT_IBMF=1 SIMULATOR=1)
target_link_libraries(tests_ibmf PRIVATE Catch2 PNG::PNG)
target_compile_definitions(tests_ibmf PRIVATE GOLDEN_DIR="${CMAKE_CURRENT_LIST_DIR}/Images")
target_sources(tests_ibmf PRIVATE
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFFont.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFFontData.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/IBMFfAce.cpp
    ${TINY_FONT_ROOT}/src/IBMFDriver/RLEExtractor.cpp
)
add_test(NAME ibmf_render COMMAND tests_ibmf)

add_executable(tests_ttf ${CMAKE_CURRENT_LIST_DIR}/TestTTF.cpp ${CMAKE_CURRENT_LIST_DIR}/ImageIO.cpp)
target_include_directories(tests_ttf PRIVATE ${TINY_FONT_ROOT}/src ${TINY_FONT_ROOT}/src/UI/Fonts ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(tests_ttf PRIVATE CONFIG_FONT_TTF=1 SIMULATOR=1 CONFIG_DISPLAY_SIM_8BIT=1 CONFIG_TTF_SCREEN_RES_PER_INCH=150 CONFIG_USE_SPIRAM=0)

# Vendor freetype for TTF tests
set(FT_DISABLE_PNG ON CACHE BOOL "Disable PNG" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable Brotli" FORCE)
add_subdirectory(${TINY_FONT_ROOT}/freetype ${CMAKE_CURRENT_BINARY_DIR}/freetype)

target_link_libraries(tests_ttf PRIVATE Catch2 freetype PNG::PNG)
target_compile_definitions(tests_ttf PRIVATE GOLDEN_DIR="${CMAKE_CURRENT_LIST_DIR}/Images")
target_sources(tests_ttf PRIVATE
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFFont.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFFontData.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFCache.cpp
    ${TINY_FONT_ROOT}/src/TTFDriver/TTFNotoSansLight.cpp
)
add_test(NAME ttf_render COMMAND tests_ttf)


