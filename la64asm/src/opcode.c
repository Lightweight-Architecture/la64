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

#include <la64asm/opcode.h>
#include <stdlib.h>
#include <string.h>

const opcode_entry_t opcode_table[LA64_OPCODE_MAX + 1] = {
    /* core operations */
    { .name = "hlt",    .opcode = LA64_OPCODE_HLT,      .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "nop",    .opcode = LA64_OPCODE_NOP,      .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000 },

    /* data operations */
    { .name = "clr",    .opcode = LA64_OPCODE_CLR,      .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111 },
    { .name = "mov",    .opcode = LA64_OPCODE_MOV,      .minargs = 2, .maxargs = 2,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "swp",    .opcode = LA64_OPCODE_SWP,      .minargs = 2, .maxargs = 2,  .argmask = 0b11000000000000000000000000000000 },
    { .name = "swpz",   .opcode = LA64_OPCODE_SWPZ,     .minargs = 2, .maxargs = 2,  .argmask = 0b11000000000000000000000000000000 },
    { .name = "push",   .opcode = LA64_OPCODE_PUSH,     .minargs = 1, .maxargs = 32, .argmask = 0b00000000000000000000000000000000 },
    { .name = "pop",    .opcode = LA64_OPCODE_POP,      .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111 },
    { .name = "ldb",    .opcode = LA64_OPCODE_LDB,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "ldw",    .opcode = LA64_OPCODE_LDW,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "ldd",    .opcode = LA64_OPCODE_LDD,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "ldq",    .opcode = LA64_OPCODE_LDQ,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "stb",    .opcode = LA64_OPCODE_STB,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "stw",    .opcode = LA64_OPCODE_STW,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "std",    .opcode = LA64_OPCODE_STD,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "stq",    .opcode = LA64_OPCODE_STQ,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },

    /* alu operations */
    { .name = "add",    .opcode = LA64_OPCODE_ADD,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "sub",    .opcode = LA64_OPCODE_SUB,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "mul",    .opcode = LA64_OPCODE_MUL,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "div",    .opcode = LA64_OPCODE_DIV,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "idiv",   .opcode = LA64_OPCODE_IDIV,     .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "mod",    .opcode = LA64_OPCODE_MOD,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "inc",    .opcode = LA64_OPCODE_INC,      .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111 },
    { .name = "dec",    .opcode = LA64_OPCODE_DEC,      .minargs = 1, .maxargs = 32, .argmask = 0b11111111111111111111111111111111 },
    { .name = "not",    .opcode = LA64_OPCODE_NOT,      .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "and",    .opcode = LA64_OPCODE_AND,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "or",     .opcode = LA64_OPCODE_OR,       .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "xor",    .opcode = LA64_OPCODE_XOR,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "shr",    .opcode = LA64_OPCODE_SHR,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "shl",    .opcode = LA64_OPCODE_SHL,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "ror",    .opcode = LA64_OPCODE_ROR,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "rol",    .opcode = LA64_OPCODE_ROL,      .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },

    /* contol flow operations */
    { .name = "jmp",    .opcode = LA64_OPCODE_JMP,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "cmp",    .opcode = LA64_OPCODE_CMP,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "je",     .opcode = LA64_OPCODE_JE,       .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jne",    .opcode = LA64_OPCODE_JNE,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jlt",    .opcode = LA64_OPCODE_JLT,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jgt",    .opcode = LA64_OPCODE_JGT,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jle",    .opcode = LA64_OPCODE_JLE,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jge",    .opcode = LA64_OPCODE_JGE,      .minargs = 1, .maxargs = 1,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jz",     .opcode = LA64_OPCODE_JZ,       .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "jnz",    .opcode = LA64_OPCODE_JNZ,      .minargs = 2, .maxargs = 2,  .argmask = 0b00000000000000000000000000000000 },
    { .name = "bl",     .opcode = LA64_OPCODE_BL,       .minargs = 1, .maxargs = 32, .argmask = 0b00000000000000000000000000000000 },
    { .name = "ret",    .opcode = LA64_OPCODE_RET,      .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000 },

    /* alu v2 operations */
    { .name = "pdep",   .opcode = LA64_OPCODE_PDEP,     .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "pext",   .opcode = LA64_OPCODE_PEXT,     .minargs = 2, .maxargs = 3,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "bswapw", .opcode = LA64_OPCODE_BSWAPW,   .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "bswapd", .opcode = LA64_OPCODE_BSWAPD,   .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000 },
    { .name = "bswapq", .opcode = LA64_OPCODE_BSWAPQ,   .minargs = 1, .maxargs = 1,  .argmask = 0b10000000000000000000000000000000 },

    /* contol flow v2 operations */
    { .name = "iret",    .opcode = LA64_OPCODE_IRET,    .minargs = 0, .maxargs = 0,  .argmask = 0b00000000000000000000000000000000 }
};

const opcode_entry_t *opcode_from_string(const char *name)
{
    /* null pointer check */
    if(name == NULL)
    {
        return NULL;
    }

    /* iterating through table */
    for(unsigned char opcode = 0x00; opcode < LA64_OPCODE_MAX + 1; opcode++)
    {
        /* check if opcode name matches */
        if(strcmp(opcode_table[opcode].name, name) == 0)
        {
            return &opcode_table[opcode];
        }
    }

    /* shouldnt happen if code is correct */
    return NULL;
}

bool opcode_arg_accepts_reg_only(const opcode_entry_t *opce,
                                 uint8_t arg)
{
    /* null pointer check */
    if(opce == NULL)
    {
        return false;
    }

    /* lol how tiny that operation is */
    return (opce->argmask & (1u << (31 - arg))) != 0;
}