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

void print_usage(const char *binary_name)
{
    /* null pointer check */
    if(binary_name == NULL)
    {
        return;
    }
    else
    {
        fprintf(stderr, "Usage: %s -r <la16 boot image>\n", binary_name);
    }
}

int main(int argc, char *argv[])
{
    /* checking for sufficient arguments */
    if(argc < 2)
    {
        print_usage(argv[0]);
        return 1;
    }
    else if(strcmp(argv[1], "-r") == 0)
    {
        /* creating new la16 virtual machine */
        la64_machine_t *machine = la64_machine_alloc(0xFFFF);

        printf("[bios] memory size: %llu bytes\n", machine->memory->memory_size);

        /* loading boot image into memory of virtual machine */
        if(!la64_memory_load_image(machine->memory, argv[2]))
        {
            return 1;
        }

        printf("[bios] reading boot image header\n");

        /*
         * getting entry point of boot image of virtual machine
         * and setting program pointer of first core to it
         */
        *(machine->core[0]->pc) = *((la64_memory_address_t*)&machine->memory->memory[0x00]);

        printf("[bios] found entry point @ 0x%llx\n", *(machine->core[0]->pc));

        /* setting stack pointer of  */
        *(machine->core[0]->sp) = machine->memory->memory_size - 2;

        printf("[bios] set stack pointer @ 0x%llx\n", *(machine->core[0]->sp));
        printf("[exec] executing core\n");

        /* executing virtual machines 1st core TODO: Implement threading */
        la64_core_execute(machine->core[0]);

        /* deallocating machine */
        la64_machine_dealloc(machine);
    }
    else
    {
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
