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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <la64/machine.h>

#include <lautils/bitwalker.h>

/* costs 8 bytes */
void la64_insert_entry(bitwalker_t *bw)
{
    /* write opcode */
    bitwalker_write(bw, 0x8, 64);
}

/* costs 2 bytes */
void la64_insert_nop(bitwalker_t *bw)
{
    /* write opcode */
    bitwalker_write(bw, LA64_OPCODE_NOP, 8);
    
    /* write end */
    bitwalker_write(bw, LA64_PARAMETER_CODING_INSTR_END, 3);

    /* skip rest of bits */
    bitwalker_skip(bw, 5);
}

/* costs 3 bytes */
void la64_insert_mov(bitwalker_t *bw)
{
    /* write opcode */
    bitwalker_write(bw, LA64_OPCODE_MOV, 8);

    /* write register parameter */
    bitwalker_write(bw, LA64_PARAMETER_CODING_REG, 3);
    bitwalker_write(bw, LA64_REGISTER_R0, 5);

    /* write intermediate 8 parameter */
    bitwalker_write(bw, LA64_PARAMETER_CODING_IMM8, 3);
    bitwalker_write(bw, 27, 8);
    
    /* write end */
    bitwalker_write(bw, LA64_PARAMETER_CODING_INSTR_END, 3);

    /* skip rest of bits */
    bitwalker_skip(bw, 2);
}

int main(int argc, char *argv[])
{
    /* creating new la16 virtual machine */
    la64_machine_t *machine = la64_machine_alloc(0xFFFFFF);

    printf("[bios] memory size: %llu bytes\n", machine->memory->memory_size);

    /* init bitwalker */
    bitwalker_t bw;
    bitwalker_init(&bw, machine->memory->memory, 1000, BW_LITTLE_ENDIAN);

    /* write a couple of instructions for testing */
    la64_insert_entry(&bw);
    la64_insert_nop(&bw);
    la64_insert_mov(&bw);
    la64_insert_nop(&bw);

    /*
     * getting entry point of boot image of virtual machine
     * and setting program pointer of first core to it
     */
    *(machine->core[0]->pc) = *((la64_memory_address_t*)&machine->memory->memory[0x0]);

    printf("[bios] found entry point @ 0x%llx\n", *(machine->core[0]->pc));

    /* setting stack pointer of  */
    *(machine->core[0]->sp) = machine->memory->memory_size - 2;

    printf("[bios] set stack pointer @ 0x%llx\n", *(machine->core[0]->sp));
    printf("[exec] executing core\n");

    /* executing virtual machines 1st core TODO: Implement threading */
    la64_core_execute(machine->core[0]);

    /* deallocating machine */
    la64_machine_dealloc(machine);

    return 0;
}
