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

int main(int argc, char *argv[])
{
    if(argc < 2 || argv == NULL || argv[1] == NULL)
    {
        goto usage;
    }

    /* creating new la16 virtual machine */
    la64_machine_t *machine = la64_machine_alloc(0xFFFFFF);

    printf("[boot] memory size: %llu bytes\n", machine->memory->memory_size);

    /* load boot image */
    if(!la64_memory_load_image(machine->memory, argv[1]))
    {
        goto usage;
    }

    /*
     * getting entry point of boot image of virtual machine
     * and setting program pointer of first core to it
     */
    machine->core->rl[LA64_REGISTER_PC] = *((la64_memory_address_t*)&machine->memory->memory[0x0]);

    printf("[boot] found entry point @ 0x%llx\n", machine->core->rl[LA64_REGISTER_PC]);

    /* setting stack pointer of  */
    machine->core->rl[LA64_REGISTER_SP] = machine->memory->memory_size - 8;

    printf("[boot] set stack pointer @ 0x%llx\n", machine->core->rl[LA64_REGISTER_SP]);
    printf("[exec] executing core\n");

    /* executing virtual machines 1st core TODO: Implement threading */
    la64_core_execute(machine->core);

    /* deallocating machine */
    la64_machine_dealloc(machine);
    
    return 0;

usage:
    printf("%s <boot image>\n", (argv == NULL || argv[0] == NULL) ? "(nil)" : argv[0]);
    return 1;
}
