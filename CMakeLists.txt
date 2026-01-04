cmake_minimum_required(VERSION 3.16)

project(LA64 LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    lautils
    GIT_REPOSITORY https://github.com/lightweight-architecture/lautils.git
    GIT_TAG main
)

FetchContent_MakeAvailable(lautils)

add_executable(la64
    src/main.c

    src/core.c
    src/machine.c
    src/memory.c
    src/mmio.c
    src/timer.c
    src/interrupt.c
    src/uart.c
    src/rtc.c

    src/instruction/core.c
    src/instruction/data.c
    src/instruction/alu.c
    src/instruction/ctrl.c
)

target_include_directories(la64
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(la64
    PRIVATE lautils
)

add_library(la64_headers INTERFACE)

target_include_directories(la64_headers
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(la64 PRIVATE c_std_99)
