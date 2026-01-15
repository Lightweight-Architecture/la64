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

#include <la64/device/platform.h>
#include <la64/machine.h>

la64_platform_t *la64_platform_alloc(la64_core_t *core)
{
    if(core == NULL)
    {
        return NULL;
    }

    la64_platform_t *p = calloc(1, sizeof(la64_platform_t));
    
    if(p == NULL)
    {
        return NULL;
    }

    p->core = core;
    p->on = true;

    return p;
}

void la64_platform_dealloc(la64_platform_t *p)
{
    if(p == NULL)
    {
        return;
    }

    free(p);
}

uint64_t la64_platform_read(la64_core_t *core, void *device, uint64_t offset, int size)
{
    if(device == NULL)
    {
        return 0;
    }

    la64_platform_t *p = (la64_platform_t*)device;

    return p->on;
}

void la64_platform_write(la64_core_t *core, void *device, uint64_t offset, uint64_t value, int size)
{
    if(device == NULL)
    {
        return;
    }

    la64_platform_t *p = (la64_platform_t*)device;

    p->on = value;

    if(!p->on)
    {
        /* to be fully implemented */
        la64_machine_dealloc(p->core->machine);
        exit(0);
    }
}