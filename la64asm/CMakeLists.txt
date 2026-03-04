cmake_minimum_required(VERSION 3.16)

project(LA64ASM LANGUAGES C)

add_executable(la64asm
    src/main.c
    src/cmptok.c
    src/code.c
    src/emit.c
    src/compiler.c
    src/label.c
    src/opcode.c
    src/register.c
    src/section.c
    src/macro.c
    src/diag.c
)

target_include_directories(la64asm
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(la64asm
    PRIVATE lautils
)

target_compile_features(la64asm PRIVATE c_std_99)

install(TARGETS la64asm
    RUNTIME DESTINATION bin
)