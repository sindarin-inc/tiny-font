cmake_minimum_required(VERSION 3.16)

set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")
message("Using ${SDKCONFIG}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_tiny-font)
