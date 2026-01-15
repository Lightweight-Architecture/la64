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
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <la64/core.h>
#include <la64/memory.h>
#include <la64/machine.h>

#include <la64/device/interrupt.h>
#include <la64/device/timer.h>

#include <la64/instruction/core.h>
#include <la64/instruction/data.h>
#include <la64/instruction/alu.h>
#include <la64/instruction/ctrl.h>

#include <lautils/bitwalker.h>

la64_opfunc_t opfunc_table[LA64_OPCODE_MAX + 1] = {
    /* core operations */
    la64_op_hlt,
    la64_op_nop,

    /* data operations */
    la64_op_clr,
    la64_op_mov,
    la64_op_swp,
    la64_op_swpz,
    la64_op_push,
    la64_op_pop,
    la64_op_ldb,
    la64_op_ldw,
    la64_op_ldd,
    la64_op_ldq,
    la64_op_stb,
    la64_op_stw,
    la64_op_std,
    la64_op_stq,

    /* arithmetic operations */
    la64_op_add,
    la64_op_sub,
    la64_op_mul,
    la64_op_div,
    la64_op_idiv,
    la64_op_mod,
    la64_op_inc,
    la64_op_dec,
    la64_op_not,
    la64_op_and,
    la64_op_or,
    la64_op_xor,
    la64_op_shr,
    la64_op_shl,
    la64_op_ror,
    la64_op_rol,
    
    /* control flow operations */
    la64_op_jmp,
    la64_op_cmp,
    la64_op_je,
    la64_op_jne,
    la64_op_jlt,
    la64_op_jgt,
    la64_op_jle,
    la64_op_jge,
    la64_op_jz,
    la64_op_jnz,
    la64_op_bl,
    la64_op_ret,

    /* alu v2 operations */
    la64_op_pdep,
    la64_op_pext,
    la64_op_bswapw,
    la64_op_bswapd,
    la64_op_bswapq
};

la64_core_t *la64_core_alloc()
{
    /* allocate a brand new core */
    la64_core_t *core = calloc(1, sizeof(la64_core_t));

    return core;
}

void la64_core_dealloc(la64_core_t *core)
{
    /* release core */
    free(core);
}

static void la64_core_decode_instruction_at_pc(la64_core_t *core)
{
    /* reset operation structure */
    memset(&(core->op), 0, sizeof(la64_operation_t));

    /* accessing memory */
    void *iptr = la64_memory_access(core, core->rl[LA64_REGISTER_PC], 100);

    /* null pointer check */
    if(iptr == NULL)
    {
        core->exception = LA64_EXCEPTION_BAD_ACCESS;
        return;
    }

    /* preparing bitwalker */
    bitwalker_t bw;
    bitwalker_init_read(&bw, iptr, 32, BW_LITTLE_ENDIAN);

    /* getting opcode */
    core->op.op = (uint8_t)bitwalker_read(&bw, 8);

    /* making shortcuts for argless opcodes */
    switch(core->op.op)
    {
        case LA64_OPCODE_HLT:
        case LA64_OPCODE_NOP:
        case LA64_OPCODE_RET:
            core->op.ilen = 1;
            goto skip_parsing;
        default:
            break;
    }

    /* parsing loop */
    bool reached_end = false;
    for(uint8_t i = 0; i < 32 && !reached_end; i++)
    {
        /* next mode */
        uint8_t mode = (uint8_t)bitwalker_read(&bw, 3);

        /* switch through modes */
        switch(mode)
        {
            case LA64_PARAMETER_CODING_INSTR_END:
                reached_end = true;
                break;
            case LA64_PARAMETER_CODING_REG:
                core->op.param[core->op.param_cnt] = &(core->rl[(uint8_t)bitwalker_read(&bw, 5)]);
                core->op.param_cnt++;
                break;
            case LA64_PARAMETER_CODING_IMM8:
                core->op.imm[core->op.param_cnt] = (uint8_t)bitwalker_read(&bw, 8);
                core->op.param[core->op.param_cnt] = &(core->op.imm[core->op.param_cnt]);
                core->op.param_cnt++;
                break;
            case LA64_PARAMETER_CODING_IMM16:
                core->op.imm[core->op.param_cnt] = (uint16_t)bitwalker_read(&bw, 16);
                core->op.param[core->op.param_cnt] = &(core->op.imm[core->op.param_cnt]);
                core->op.param_cnt++;
                break;
            case LA64_PARAMETER_CODING_IMM32:
                core->op.imm[core->op.param_cnt] = (uint32_t)bitwalker_read(&bw, 32);
                core->op.param[core->op.param_cnt] = &(core->op.imm[core->op.param_cnt]);
                core->op.param_cnt++;
                break;
            case LA64_PARAMETER_CODING_IMM64:
                core->op.imm[core->op.param_cnt] = (uint64_t)bitwalker_read(&bw, 64);
                core->op.param[core->op.param_cnt] = &(core->op.imm[core->op.param_cnt]);
                core->op.param_cnt++;
                break;
            default:
                core->exception = LA64_EXCEPTION_BAD_INSTRUCTION;
                reached_end = true;
                return;
        }
    }

skip_parsing:

    /* finding out how many steps the the program counter has to jump */
    core->op.ilen = bitwalker_bytes_used(&bw);

    return;
}

static void *la64_core_execute_thread(void *arg)
{
    /* null pointer check */
    if(arg == NULL)
    {
        return NULL;
    }

    /* cast argument to core */
    la64_core_t *core = arg;

    /* setting properties */
    core->exception = LA64_EXCEPTION_NONE;
    core->halted = false;

    while(1)
    {
        /* checking if core is halted, if so simply skip execution */
        if(core->halted)
        {
            /* yield cpu to not burn it */
            usleep(100);
            goto skip_execution;
        }

        /* checking for any exception */
        switch(core->exception)
        {
            case LA64_EXCEPTION_BAD_ACCESS:
                goto switch_raise_isoftware;
            case LA64_EXCEPTION_PERMISSION:
                goto switch_raise_isoftware;
            case LA64_EXCEPTION_BAD_INSTRUCTION:
bad_instruction_shortcut:
                goto switch_raise_isoftware;
            case LA64_EXCEPTION_BAD_ARITHMETIC:
switch_raise_isoftware:
                core->halted = true;
                la64_raise_interrupt(core, LA64_IRQ_SOFTWARE);
                break;
            default:
                break;
        }

        /* decoding instruction */
        la64_core_decode_instruction_at_pc(core);

        /* sanity check */
        if(core->op.op > LA64_OPCODE_MAX || opfunc_table[core->op.op] == NULL)
        {
            goto bad_instruction_shortcut;
        }

        /* executing instruction */
        opfunc_table[core->op.op](core);

        /* incrementing program counter by instruction size */
        core->rl[LA64_REGISTER_PC] += core->op.ilen;

        /* interrupt controller checking routine starts here */
skip_execution:

        /* check and handle pending interrupts */
        if(core->machine && core->machine->intc && la64_intc_pending(core->machine->intc))
        {
            la64_intc_check(core);
        }

        /* tick the timer always */
        if(core->machine && core->machine->timer)
        {
            extern uint64_t la64_get_host_cycles(void);
            la64_timer_tick(core->machine->timer, la64_get_host_cycles());
        }
    }

    //core->runs = 0b00000000;
    return NULL;
}


void la64_core_execute(la64_core_t *core)
{
    /* core running check */
    /*if(core->runs)
    {
        return;
    }*/

    /* invoking execution */
    pthread_t pthread;
    pthread_create(&pthread, NULL, la64_core_execute_thread, (void*)core);
    pthread_join(pthread, NULL);
}

void la64_core_terminate(la64_core_t *core)
{
    /* setting termination flag (TODO: needs atomics) */
    //core->term = 0b00000001;
}