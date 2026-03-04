cmake_minimum_required(VERSION 3.16)

project(lautils
    VERSION 1.0.0
    LANGUAGES C
)

add_library(lautils STATIC
    src/bitwalker.c
    src/parser.c
)

target_include_directories(lautils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(lautils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(lautils PUBLIC c_std_23)

if (MSVC)
    target_compile_options(lautils PRIVATE /W4)
else()
    target_compile_options(lautils PRIVATE -Wall -Wextra -Wpedantic)
endif()