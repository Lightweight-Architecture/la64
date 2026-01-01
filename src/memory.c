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

#include <la64/memory.h>
#include <la64/core.h>
#include <la64/machine.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

la64_memory_t *la64_memory_alloc(la64_memory_size_t size)
{
    /* allocating memory */
    la64_memory_t *memory = calloc(1, sizeof(la64_memory_t));

    /* null pointer check */
    if(memory == NULL)
    {
        return NULL;
    }

    /* allocate raw memory (using mmap for larger sizes, better than heap in this case) */
    memory->memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    /* null pointer and sanity check */
    if(memory->memory == MAP_FAILED ||
       memory->memory == NULL)
    {
        free(memory);
        return NULL;
    }

    /* setting property */
    memory->memory_size = size;

    return memory;
}

void la64_memory_dealloc(la64_memory_t *memory)
{
    /* null pointer check */
    if(memory == NULL)
    {
        return;
    }

    /* release that shit */
    munmap(memory->memory, memory->memory_size);
    free(memory);
}

bool la64_memory_load_image(la64_memory_t *memory,
                            const char *image_path)
{
    /* open boot image */
    int fd = open(image_path, O_RDONLY);

    /* checking file descriptor */
    if(fd == -1)
    {
        printf("[boot] failed to open boot image at path %s\n", image_path);
        return false;
    }

    /* gather size of boot image */
    struct stat image_stat;
    fstat(fd, &image_stat);
    size_t image_size = image_stat.st_size;

    /* checking if memory is big enough for our memory */
    if(image_size > memory->memory_size)
    {
        printf("[boot] error: boot image is too large\n");
        return false;
    }

    /* loading boot image into memory */
    read(fd, memory->memory, image_size);

    printf("[boot] loaded boot image: %zu bytes\n", image_size);

    close(fd);

    return true;
}

size_t size_from_access_type(la64_memory_access_size_t access_size)
{
    switch(access_size)
    {
        case la64MemoryAccessSizeByte:
            return sizeof(uint8_t);
        case la64MemoryAccessSizeWord:
            return sizeof(uint16_t);
        case la64MemoryAccessSizeDoubleWord:
            return sizeof(uint32_t);
        case la64MemoryAccessSizeQuadWord:
            return sizeof(uint64_t);
        case la64MemoryAccessSizeInstruction:
            return (sizeof(uint64_t) * 45);         /* just to make sure */
        default:
            return 0;
    }
}

void *la64_memory_access(la64_core_t *core,
                         uint64_t addr,
                         la64_memory_access_size_t access_size)
{
    /* null pointer check */
    if(core == NULL || core->machine == NULL || core->machine->memory == NULL)
    {
        return NULL;
    }

    /* getting size */
    size_t size = size_from_access_type(access_size);

    /* null size check */
    if(size == 0)
    {
        return NULL;
    }

    /* get end */
    uint64_t addr_end = addr + size;

    /* wrap around check */
    if(addr >= addr_end ||
       core->machine->memory->memory_size < addr_end)
    {
        return NULL;
    }

    return &(core->machine->memory->memory[addr]);
}