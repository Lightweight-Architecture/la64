cmake_minimum_required(VERSION 3.16)

project(LA64VM LANGUAGES C)

add_executable(la64vm
    src/main.c

    src/core.c
    src/machine.c
    src/memory.c
    src/mmio.c

    src/device/timer.c
    src/device/interrupt.c
    src/device/uart.c
    src/device/rtc.c
    src/device/mc.c
    src/device/platform.c
    src/device/display.c

    src/instruction/core.c
    src/instruction/data.c
    src/instruction/alu.c
    src/instruction/ctrl.c
)

target_include_directories(la64vm
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(la64vm
    PRIVATE lautils
)

if(UNIX AND NOT APPLE)
    target_link_libraries(la64vm
        PRIVATE glfw
        PRIVATE GLEW
        PRIVATE GL
    )
endif()

if(APPLE)
    target_sources(la64vm
        PRIVATE
            src/device/display.m
    )

    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)

    target_link_libraries(la64vm
        PRIVATE
            ${COCOA_FRAMEWORK}
            ${OPENGL_FRAMEWORK}
    )
endif()

target_compile_features(la64vm PRIVATE c_std_99)

install(TARGETS la64vm
    RUNTIME DESTINATION bin
)
