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

#include <la64/machine.h>

#include <la64/device/mc.h>

#include <stdlib.h>

la64_mc_t *la64_mc_alloc(la64_core_t *core)
{
    /* null pointer check */
    if(core == NULL)
    {
        return NULL;
    }

    /* allocate memory controller */
    la64_mc_t *mc = calloc(1, sizeof(la64_mc_t));

    /* null pointer check again */
    if(mc == NULL)
    {
        return NULL;
    }

    /* setting core */
    mc->core = core;

    return mc;
}

void la64_mc_dealloc(la64_mc_t *mc)
{
    /* null pointer check */
    if(mc == NULL)
    {
        return;
    }

    free(mc);
}

uint64_t la64_mc_read(void *device,
                      uint64_t offset,
                      int size)
{
    /* null pointer check */
    if(device == NULL)
    {
        return 0;
    }

    /* getting device */
    la64_mc_t *mc = device;

    /* returning memory size */
    return mc->core->machine->memory->memory_size;
}

void la64_mc_write(void *device,
                   uint64_t offset,
                   uint64_t value,
                   int size)
{
    /* ro device */
    return;
}