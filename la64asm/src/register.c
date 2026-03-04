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

#include <la64asm/register.h>
#include <stdlib.h>
#include <string.h>

register_entry_t register_table[] = {
    { .name = "pc", .reg = LA64_REGISTER_PC },
    { .name = "sp", .reg = LA64_REGISTER_SP },
    { .name = "fp", .reg = LA64_REGISTER_FP },
    { .name = "cf", .reg = LA64_REGISTER_CF },

    { .name = "r0", .reg = LA64_REGISTER_R0 },
    { .name = "r1", .reg = LA64_REGISTER_R1 },
    { .name = "r2", .reg = LA64_REGISTER_R2 },
    { .name = "r3", .reg = LA64_REGISTER_R3 },
    { .name = "r4", .reg = LA64_REGISTER_R4 },
    { .name = "r5", .reg = LA64_REGISTER_R5 },
    { .name = "r6", .reg = LA64_REGISTER_R6 },
    { .name = "r7", .reg = LA64_REGISTER_R7 },
    { .name = "r8", .reg = LA64_REGISTER_R8 },
    { .name = "r9", .reg = LA64_REGISTER_R9 },
    { .name = "r10", .reg = LA64_REGISTER_R10 },
    { .name = "r11", .reg = LA64_REGISTER_R11 },
    { .name = "r12", .reg = LA64_REGISTER_R12 },
    { .name = "r13", .reg = LA64_REGISTER_R13 },
    { .name = "r14", .reg = LA64_REGISTER_R14 },
    { .name = "r15", .reg = LA64_REGISTER_R15 },
    { .name = "r16", .reg = LA64_REGISTER_R16 },
    { .name = "r17", .reg = LA64_REGISTER_R17 },
    { .name = "r18", .reg = LA64_REGISTER_R18 },
    { .name = "r19", .reg = LA64_REGISTER_R19 },
    { .name = "r20", .reg = LA64_REGISTER_R20 },
    { .name = "r21", .reg = LA64_REGISTER_R21 },
    { .name = "r22", .reg = LA64_REGISTER_R22 },
    { .name = "r23", .reg = LA64_REGISTER_R23 },
    { .name = "r24", .reg = LA64_REGISTER_R24 },
    { .name = "r25", .reg = LA64_REGISTER_R25 },
    { .name = "r26", .reg = LA64_REGISTER_R26 },
    { .name = "rr", .reg = LA64_REGISTER_RR }
};

register_entry_t control_register_table[] = {
    { .name = "cr0", .reg = LA64_CONTROL_REGISTER_CR0 },
    { .name = "cr1", .reg = LA64_CONTROL_REGISTER_CR1 },
    { .name = "cr2", .reg = LA64_CONTROL_REGISTER_CR2 },
    { .name = "cr3", .reg = LA64_CONTROL_REGISTER_CR3 },
    { .name = "cr4", .reg = LA64_CONTROL_REGISTER_CR4 },
    { .name = "cr5", .reg = LA64_CONTROL_REGISTER_CR5 },
    { .name = "cr6", .reg = LA64_CONTROL_REGISTER_CR6 },
    { .name = "cr7", .reg = LA64_CONTROL_REGISTER_CR7 },
    { .name = "cr8", .reg = LA64_CONTROL_REGISTER_CR8 },
    { .name = "cr9", .reg = LA64_CONTROL_REGISTER_CR9 },
    { .name = "cr10", .reg = LA64_CONTROL_REGISTER_CR10 },
    { .name = "cr11", .reg = LA64_CONTROL_REGISTER_CR11 },
    { .name = "cr12", .reg = LA64_CONTROL_REGISTER_CR12 },
    { .name = "cr13", .reg = LA64_CONTROL_REGISTER_CR13 },
    { .name = "cr14", .reg = LA64_CONTROL_REGISTER_CR14 },
    { .name = "cr15", .reg = LA64_CONTROL_REGISTER_CR15 },
    { .name = "cr16", .reg = LA64_CONTROL_REGISTER_CR16 },
    { .name = "cr17", .reg = LA64_CONTROL_REGISTER_CR17 },
    { .name = "cr18", .reg = LA64_CONTROL_REGISTER_CR18 },
    { .name = "cr19", .reg = LA64_CONTROL_REGISTER_CR19 },
    { .name = "cr20", .reg = LA64_CONTROL_REGISTER_CR20 },
    { .name = "cr21", .reg = LA64_CONTROL_REGISTER_CR21 },
    { .name = "cr22", .reg = LA64_CONTROL_REGISTER_CR22 },
    { .name = "cr23", .reg = LA64_CONTROL_REGISTER_CR23 },
    { .name = "cr24", .reg = LA64_CONTROL_REGISTER_CR24 },
    { .name = "cr25", .reg = LA64_CONTROL_REGISTER_CR25 },
    { .name = "cr26", .reg = LA64_CONTROL_REGISTER_CR26 },
    { .name = "cr27", .reg = LA64_CONTROL_REGISTER_CR27 },
    { .name = "cr28", .reg = LA64_CONTROL_REGISTER_CR28 },
    { .name = "cr29", .reg = LA64_CONTROL_REGISTER_CR29 },
    { .name = "cr30", .reg = LA64_CONTROL_REGISTER_CR30 },
    { .name = "cr31", .reg = LA64_CONTROL_REGISTER_CR31 },
};

register_entry_t *register_from_string(const char *name)
{
    /* null pointer check */
    if(name == NULL)
    {
        return NULL;
    }

    /* iterating through table */
    for(unsigned char reg = 0x00; reg < (LA64_REGISTER_MAX + 1); reg++)
    {
        /* check if opcode name matches */
        if(strcmp(register_table[reg].name, name) == 0)
        {
            return &register_table[reg];
        }
    }

    /* shouldnt happen if code is correct */
    return NULL;
}

register_entry_t *control_register_from_string(const char *name)
{
    /* null pointer check */
    if(name == NULL)
    {
        return NULL;
    }

    /* iterating through table */
    for(unsigned char reg = 0x00; reg < (LA64_CONTROL_REGISTER_MAX + 1); reg++)
    {
        /* check if opcode name matches */
        if(strcmp(control_register_table[reg].name, name) == 0)
        {
            return &register_table[reg];
        }
    }

    /* shouldnt happen if code is correct */
    return NULL;
}