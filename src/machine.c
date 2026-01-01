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

#include <stdlib.h>
#include <la64/machine.h>

la64_machine_t *la64_machine_alloc(uint64_t memory_size)
{
    /* allocating brand new machine */
    la64_machine_t *machine = calloc(1, sizeof(la64_machine_t));

    /* null pointer check */
    if(machine == NULL)
    {
        return NULL;
    }

    /* allocate memory */
    machine->memory = la64_memory_alloc(memory_size);

    /* null pointer check */
    if(machine->memory == NULL)
    {
        /* release machine and return nothing */
        goto out_release_machine;
    }

    /* allocate new cores */
    for(uint8_t i = 0; i < 4; i++)
    {
        machine->core[i] = la64_core_alloc();

        /* null pointer check */
        if(machine->core[i] == NULL)
        {
            /* now revert core allocation */
            for(; 0 < i; i--)
            {
                la64_core_dealloc(machine->core[i]);
            }

            /* releasing */
            goto out_release_memory;
        }

        machine->core[i]->machine = machine;
    }

    return machine;

    /* much more compact error handling */
out_release_memory:
    la64_memory_dealloc(machine->memory);
out_release_machine:
    free(machine);
    return NULL;
}

void la64_machine_dealloc(la64_machine_t *machine)
{
    /* null pointer check */
    if(machine == NULL)
    {
        return;
    }

    /* release cores */
    for(unsigned char i = 0; i < 4; i++)
    {
        la64_core_dealloc(machine->core[i]);
    }

    /* release memory */
    la64_memory_dealloc(machine->memory);

    /* release machine */
    free(machine);
}
