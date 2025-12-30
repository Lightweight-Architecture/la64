/*
 * MIT License
 *
 * Copyright (c) 2024 cr4zyengineer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LA64_CORE_H
#define LA64_CORE_H

#include <la64/register.h>
#include <stdint.h>

#pragma mark - opcode

/* core operations */
#define LA64_OPCODE_HLT             0b00000000
#define LA64_OPCODE_NOP             0b00000001

/* data operations */
#define LA64_OPCODE_MOV             0b00000010
#define LA64_OPCODE_SWP             0b00000011
#define LA64_OPCODE_SWPZ            0b00000100
#define LA64_OPCODE_PUSH            0b00000101
#define LA64_OPCODE_POP             0b00000110
#define LA64_OPCODE_LDB             0b00000111
#define LA64_OPCODE_LDW             0b00001000
#define LA64_OPCODE_LDD             0b00001001
#define LA64_OPCODE_LDQ             0b00001010
#define LA64_OPCODE_STB             0b00001011
#define LA64_OPCODE_STW             0b00001100
#define LA64_OPCODE_STD             0b00001101
#define LA64_OPCODE_STQ             0b00001110

/* io operations */
#define LA64_OPCODE_IN              0b00001111
#define LA64_OPCODE_OUT             0b00010000

/* alu operations */
#define LA64_OPCODE_ADD             0b00010001
#define LA64_OPCODE_SUB             0b00010010
#define LA64_OPCODE_MUL             0b00010011
#define LA64_OPCODE_DIV             0b00010100
#define LA64_OPCODE_IDIV            0b00010101
#define LA64_OPCODE_MOD             0b00010110
#define LA64_OPCODE_INC             0b00010111
#define LA64_OPCODE_DEC             0b00011000
#define LA64_OPCODE_NOT             0b00011001
#define LA64_OPCODE_AND             0b00011010
#define LA64_OPCODE_OR              0b00011011
#define LA64_OPCODE_XOR             0b00011100
#define LA64_OPCODE_SHR             0b00011101
#define LA64_OPCODE_SHL             0b00011110
#define LA64_OPCODE_ROR             0b00011111
#define LA64_OPCODE_ROL             0b00100000

/* control flow operations */
#define LA64_OPCODE_JMP             0b00100001
#define LA64_OPCODE_CMP             0b00100010
#define LA64_OPCODE_JE              0b00100011
#define LA64_OPCODE_JNE             0b00100100
#define LA64_OPCODE_JLT             0b00100101
#define LA64_OPCODE_JGT             0b00100110
#define LA64_OPCODE_JLE             0b00100111
#define LA64_OPCODE_JGE             0b00101000
#define LA64_OPCODE_JZ              0b00101001
#define LA64_OPCODE_JNZ             0b00101010
#define LA64_OPCODE_BL              0b00101011
#define LA64_OPCODE_RET             0b00101100

#define LA64_OPCODE_MAX             LA64_OPCODE_RET

#pragma mark - parameter modes

#define LA64_PARAMETER_CODING_INSTR_END 0b000
#define LA64_PARAMETER_CODING_REG       0b001
#define LA64_PARAMETER_CODING_IMM8      0b010
#define LA64_PARAMETER_CODING_IMM16     0b011
#define LA64_PARAMETER_CODING_IMM32     0b100
#define LA64_PARAMETER_CODING_IMM64     0b101
/* leaving 0x110/0x111 open for later additions */

#pragma mark - register

#define LA64_REGISTER_PC    0b00000
#define LA64_REGISTER_SP    0b00001
#define LA64_REGISTER_FP    0b00010
#define LA64_REGISTER_CF    0b00011

#define LA64_REGISTER_R0    0b00100
#define LA64_REGISTER_R1    0b00101
#define LA64_REGISTER_R2    0b00110
#define LA64_REGISTER_R3    0b00111
#define LA64_REGISTER_R4    0b01000
#define LA64_REGISTER_R5    0b01001
#define LA64_REGISTER_R6    0b01010
#define LA64_REGISTER_R7    0b01011
#define LA64_REGISTER_R8    0b01100
#define LA64_REGISTER_R9    0b01101
#define LA64_REGISTER_R10   0b01110
#define LA64_REGISTER_R11   0b01111
#define LA64_REGISTER_R12   0b10000
#define LA64_REGISTER_R13   0b10001
#define LA64_REGISTER_R14   0b10010
#define LA64_REGISTER_R15   0b10011
#define LA64_REGISTER_R16   0b10100
#define LA64_REGISTER_R17   0b10101
#define LA64_REGISTER_R18   0b10110
#define LA64_REGISTER_R19   0b10111
#define LA64_REGISTER_R20   0b11000
#define LA64_REGISTER_R21   0b11001
#define LA64_REGISTER_R22   0b11010
#define LA64_REGISTER_R23   0b11011
#define LA64_REGISTER_R24   0b11100
#define LA64_REGISTER_R25   0b11101
#define LA64_REGISTER_R26   0b11110
#define LA64_REGISTER_RR    0b11111

#define LA64_REGISTER_MAX   LA64_REGISTER_RR

#pragma mark - compare flags

#define LA64_CMP_Z  0x1
#define LA64_CMP_L  0x2
#define LA64_CMP_G  0x4

#pragma mark - termination flags

#define LA64_TERM_NONE              0b000
#define LA64_TERM_HALT              0b001
#define LA64_TERM_BAD_ACCESS        0b010
#define LA64_TERM_PERMISSION        0b011
#define LA64_TERM_BAD_INSTRUCTION   0b100

typedef struct la64_machine la64_machine_t;

typedef struct {
    uint8_t ilen;
    uint8_t op;
    uint64_t imm[32];
    uint8_t param_cnt;
    uint64_t *param[32];
} la64_operation_t;

struct la64_core {
    /* shortcuts too special purpose registers */
    la64_register_t pc;     /* program counter */
    la64_register_t sp;     /* stack pointer */
    la64_register_t fp;     /* frame pointer */
    la64_register_t cf;     /* compare flag*/

    la64_register_t rl[LA64_REGISTER_MAX + 1];

    /* Opertion registers */
    la64_operation_t op;

    /* Exec flags */
    uint8_t runs;
    uint8_t term;

    /* Machine related things */
    la64_machine_t *machine;
};

typedef struct la64_core la64_core_t;
typedef void (*la64_opfunc_t)(la64_core_t *core);

extern la64_opfunc_t opfunc_table[LA64_OPCODE_MAX + 1];

la64_core_t *la64_core_alloc(void);
void la64_core_dealloc(la64_core_t *core);
void la64_core_execute(la64_core_t *core);
void la64_core_terminate(la64_core_t *core);

#endif /* LA64_CORE_H */
