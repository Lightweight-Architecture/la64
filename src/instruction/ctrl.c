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

#include <la64/instruction/instruction.h>
#include <la64/instruction/ctrl.h>
#include <la64/machine.h>

void la64_op_jmp(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    *(core->pc) = *(core->op.param[0]) - core->op.ilen;
}

void la64_op_cmp(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    int64_t a = (int64_t)*(core->op.param[0]);
    int64_t b = (int64_t)*(core->op.param[1]);
    
    *(core->cf) = (a == b) * LA64_CMP_Z | (a <  b) * LA64_CMP_L | (a >  b) * LA64_CMP_G;
}

void la64_op_je(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(*(core->cf) & LA64_CMP_Z)
    {
        la64_op_jmp(core);
    }
}

void la64_op_jne(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(!(*(core->cf) & LA64_CMP_Z))
    {
        la64_op_jmp(core);
    }
}

void la64_op_jlt(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(*(core->cf) & LA64_CMP_L)
    {
        la64_op_jmp(core);
    }
}

void la64_op_jgt(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(*(core->cf) & LA64_CMP_G)
    {
        la64_op_jmp(core);
    }
}

void la64_op_jle(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(*(core->cf) & LA64_CMP_L || *(core->cf) & LA64_CMP_Z)
    {
        la64_op_jmp(core);
    }
}

void la64_op_jge(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    
    if(*(core->cf) & LA64_CMP_G || *(core->cf) & LA64_CMP_Z)
    {
        la64_op_jmp(core);
    }
}

void la64_op_jz(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);
    
    if(*(core->op.param[0]) == 0)
    {
        core->op.param[0] = core->op.param[1];
        la64_op_jmp(core);
    }
}

void la64_op_jnz(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);
    
    if(*(core->op.param[0]) != 0)
    {
        core->op.param[0] = core->op.param[1];
        la64_op_jmp(core);
    }
}

void la64_push(la64_core_t *core, uint64_t val)
{
    *((uint64_t*)&(core->machine->memory->memory[*(core->sp)])) = *(core->op.param[0]);
    *(core->sp) -= 8;
}

void la64_pop(la64_core_t *core, uint64_t *val)
{
    *(core->sp) += 8;
    *(core->op.param[0]) = *((uint64_t*)&(core->machine->memory->memory[*(core->sp)]));
}

/* todo, call directly in bl via args (would be the biggest win ever) */
void la64_op_bl(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    la64_push(core, *core->rl[LA64_REGISTER_PC]);
    la64_push(core, *core->rl[LA64_REGISTER_CF]);
    la64_push(core, *core->rl[LA64_REGISTER_R0]);
    la64_push(core, *core->rl[LA64_REGISTER_R1]);
    la64_push(core, *core->rl[LA64_REGISTER_R2]);
    la64_push(core, *core->rl[LA64_REGISTER_R3]);
    la64_push(core, *core->rl[LA64_REGISTER_R4]);
    la64_push(core, *core->rl[LA64_REGISTER_R5]);
    la64_push(core, *core->rl[LA64_REGISTER_R6]);
    la64_push(core, *core->rl[LA64_REGISTER_R7]);
    la64_push(core, *core->rl[LA64_REGISTER_R8]);
    la64_push(core, *core->rl[LA64_REGISTER_R9]);
    la64_push(core, *core->rl[LA64_REGISTER_R10]);
    la64_push(core, *core->rl[LA64_REGISTER_R11]);
    la64_push(core, *core->rl[LA64_REGISTER_R12]);
    la64_push(core, *core->rl[LA64_REGISTER_R13]);
    la64_push(core, *core->rl[LA64_REGISTER_R14]);
    la64_push(core, *core->rl[LA64_REGISTER_R15]);
    la64_push(core, *core->rl[LA64_REGISTER_R16]);
    la64_push(core, *core->rl[LA64_REGISTER_R17]);
    la64_push(core, *core->rl[LA64_REGISTER_R18]);
    la64_push(core, *core->rl[LA64_REGISTER_R19]);
    la64_push(core, *core->rl[LA64_REGISTER_R20]);
    la64_push(core, *core->rl[LA64_REGISTER_R21]);
    la64_push(core, *core->rl[LA64_REGISTER_R22]);
    la64_push(core, *core->rl[LA64_REGISTER_R23]);
    la64_push(core, *core->rl[LA64_REGISTER_R24]);
    la64_push(core, *core->rl[LA64_REGISTER_R25]);
    la64_push(core, *core->rl[LA64_REGISTER_R26]);
    la64_push(core, *core->rl[LA64_REGISTER_FP]);
    *(core->fp) = *(core->sp);
    *(core->pc) = *(core->op.param[0]) - 4;
}

void la64_op_ret(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);
    *(core->sp) = *(core->fp);
    la64_pop(core, core->rl[LA64_REGISTER_FP]);
    la64_pop(core, core->rl[LA64_REGISTER_R24]);
    la64_pop(core, core->rl[LA64_REGISTER_R23]);
    la64_pop(core, core->rl[LA64_REGISTER_R22]);
    la64_pop(core, core->rl[LA64_REGISTER_R21]);
    la64_pop(core, core->rl[LA64_REGISTER_R20]);
    la64_pop(core, core->rl[LA64_REGISTER_R19]);
    la64_pop(core, core->rl[LA64_REGISTER_R18]);
    la64_pop(core, core->rl[LA64_REGISTER_R17]);
    la64_pop(core, core->rl[LA64_REGISTER_R16]);
    la64_pop(core, core->rl[LA64_REGISTER_R15]);
    la64_pop(core, core->rl[LA64_REGISTER_R14]);
    la64_pop(core, core->rl[LA64_REGISTER_R13]);
    la64_pop(core, core->rl[LA64_REGISTER_R12]);
    la64_pop(core, core->rl[LA64_REGISTER_R11]);
    la64_pop(core, core->rl[LA64_REGISTER_R10]);
    la64_pop(core, core->rl[LA64_REGISTER_R9]);
    la64_pop(core, core->rl[LA64_REGISTER_R8]);
    la64_pop(core, core->rl[LA64_REGISTER_R7]);
    la64_pop(core, core->rl[LA64_REGISTER_R6]);
    la64_pop(core, core->rl[LA64_REGISTER_R5]);
    la64_pop(core, core->rl[LA64_REGISTER_R4]);
    la64_pop(core, core->rl[LA64_REGISTER_R3]);
    la64_pop(core, core->rl[LA64_REGISTER_R2]);
    la64_pop(core, core->rl[LA64_REGISTER_R1]);
    la64_pop(core, core->rl[LA64_REGISTER_R0]);
    la64_pop(core, core->rl[LA64_REGISTER_CF]);
    la64_pop(core, core->rl[LA64_REGISTER_PC]);
}