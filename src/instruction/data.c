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
#include <la64/instruction/instruction.h>
#include <la64/instruction/data.h>
#include <la64/machine.h>
#include <la64/memory.h>

void la64_op_mov(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    /* performing move */
    *(core->op.param[0]) = *(core->op.param[1]);
}

void la64_op_swp(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    /* performing swap */
    uint64_t param_backup = *(core->op.param[0]);
    *(core->op.param[0]) = *(core->op.param[1]);
    *(core->op.param[1]) = param_backup;
}

void la64_op_swpz(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    /* performing zero out swap */
    *(core->op.param[0]) = *(core->op.param[1]);
    *(core->op.param[1]) = 0;
}

void la64_op_push(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    void *ptr = la64_memory_access(core, core->rl[LA64_REGISTER_SP], la64MemoryAccessSizeQuadWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *((uint64_t*)ptr) = *(core->op.param[0]);
    core->rl[LA64_REGISTER_SP] -= 8;
}

void la64_op_pop(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);

    core->rl[LA64_REGISTER_SP] += 8;

    void *ptr = la64_memory_access(core, core->rl[LA64_REGISTER_SP], la64MemoryAccessSizeQuadWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *(core->op.param[0]) = *((uint64_t*)ptr);
}

void la64_op_ldb(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[1]), la64MemoryAccessSizeByte);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *(core->op.param[0]) = *((uint8_t*)ptr);
}

void la64_op_ldw(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[1]), la64MemoryAccessSizeWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *(core->op.param[0]) = *((uint16_t*)ptr);
}

void la64_op_ldd(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[1]), la64MemoryAccessSizeDoubleWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *(core->op.param[0]) = *((uint32_t*)ptr);
}

void la64_op_ldq(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[1]), la64MemoryAccessSizeQuadWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *(core->op.param[0]) = *((uint64_t*)ptr);
}

void la64_op_stb(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[0]), la64MemoryAccessSizeByte);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *((uint8_t*)ptr) = *(core->op.param[1]);
}

void la64_op_stw(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[0]), la64MemoryAccessSizeWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *((uint16_t*)ptr) = *(core->op.param[1]);
}

void la64_op_std(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[0]), la64MemoryAccessSizeDoubleWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *((uint32_t*)ptr) = *(core->op.param[1]);
}

void la64_op_stq(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    void *ptr = la64_memory_access(core, *(core->op.param[0]), la64MemoryAccessSizeQuadWord);

    if(ptr == NULL)
    {
        core->term = LA64_TERM_BAD_ACCESS;
        return;
    }

    *((uint64_t*)ptr) = *(core->op.param[1]);
}