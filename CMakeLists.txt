file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IBMFDriver/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TTFDriver/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Misc/*.cpp
)

if(DEFINED ESP_PLATFORM AND ESP_PLATFORM)
    set(compile_options
        -includesdkconfig.h
    )

    idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS "src"
                        REQUIRES freetype)

    target_compile_options(${COMPONENT_TARGET} PUBLIC ${compile_options})
else()
    add_library(tiny-font STATIC
        ${SOURCES}
    )
    target_include_directories(tiny-font PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
endif()



project(tiny-font)
