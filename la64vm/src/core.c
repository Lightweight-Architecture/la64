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

#include <la64vm/core.h>
#include <la64vm/memory.h>
#include <la64vm/machine.h>

#include <la64vm/device/interrupt.h>
#include <la64vm/device/timer.h>

#include <la64vm/instruction/core.h>
#include <la64vm/instruction/data.h>
#include <la64vm/instruction/alu.h>
#include <la64vm/instruction/ctrl.h>

#include <lautils/bitwalker.h>

#if defined(__APPLE__)
#include <CoreFoundation/CFRunLoop.h>
#endif /* __APPLE__ */

la64_opfunc_t opfunc_table[] = {
    /* core operations */
    la64_op_hlt,
    la64_op_nop,

    /* data operations */
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
    la64_op_not,
    la64_op_neg,
    la64_op_and,
    la64_op_or,
    la64_op_xor,
    la64_op_shr,
    la64_op_shl,
    la64_op_sar,
    la64_op_ror,
    la64_op_rol,
    la64_op_pdep,
    la64_op_pext,
    la64_op_bswapw,
    la64_op_bswapd,
    la64_op_bswapq,

    /* control flow operations */
    la64_op_b,
    la64_op_cmp,
    la64_op_be,
    la64_op_bne,
    la64_op_blt,
    la64_op_bgt,
    la64_op_ble,
    la64_op_bge,
    la64_op_bz,
    la64_op_bnz,
    la64_op_bl,
    la64_op_ret,
    la64_op_iret
};

static const uint8_t opcode_maxargs[256] = {
    [LA64_OPCODE_HLT] = 0,
    [LA64_OPCODE_NOP] = 0,

    [LA64_OPCODE_MOV] = 2,
    [LA64_OPCODE_SWP] = 2,
    [LA64_OPCODE_SWPZ] = 2,
    [LA64_OPCODE_PUSH] = 32,
    [LA64_OPCODE_POP] = 32,
    [LA64_OPCODE_LDB] = 2,
    [LA64_OPCODE_LDW] = 2,
    [LA64_OPCODE_LDD] = 2,
    [LA64_OPCODE_LDQ] = 2,
    [LA64_OPCODE_STB] = 2,
    [LA64_OPCODE_STW] = 2,
    [LA64_OPCODE_STD] = 2,
    [LA64_OPCODE_STQ] = 2,

    [LA64_OPCODE_ADD] = 3,
    [LA64_OPCODE_SUB] = 3,
    [LA64_OPCODE_MUL] = 3,
    [LA64_OPCODE_DIV] = 3,
    [LA64_OPCODE_IDIV] = 3,
    [LA64_OPCODE_MOD] = 3,
    [LA64_OPCODE_NOT] = 32,
    [LA64_OPCODE_NEG] = 32,
    [LA64_OPCODE_AND] = 3,
    [LA64_OPCODE_OR] = 3,
    [LA64_OPCODE_XOR] = 3,
    [LA64_OPCODE_SHR] = 3,
    [LA64_OPCODE_SHL] = 3,
    [LA64_OPCODE_SAR] = 3,
    [LA64_OPCODE_ROR] = 3,
    [LA64_OPCODE_ROL] = 3,
    [LA64_OPCODE_PDEP] = 3,
    [LA64_OPCODE_PEXT] = 3,
    [LA64_OPCODE_BSWAPW] = 1,
    [LA64_OPCODE_BSWAPD] = 1,
    [LA64_OPCODE_BSWAPQ] = 1,

    [LA64_OPCODE_B] = 1,
    [LA64_OPCODE_CMP] = 2,
    [LA64_OPCODE_BE] = 1,
    [LA64_OPCODE_BNE] = 1,
    [LA64_OPCODE_BLT] = 1,
    [LA64_OPCODE_BGT] = 1,
    [LA64_OPCODE_BLE] = 1,
    [LA64_OPCODE_BGE] = 1,
    [LA64_OPCODE_BZ] = 2,
    [LA64_OPCODE_BNZ] = 2,
    [LA64_OPCODE_BL] = 32,
    [LA64_OPCODE_RET] = 0,
    [LA64_OPCODE_IRET] = 0,
};

la64_core_t *la64_core_alloc()
{
    /* allocate a brand new core */
    la64_core_t *core = malloc(sizeof(la64_core_t));

    if(core == NULL)
    {
        return NULL;
    }

    bzero(core, sizeof(la64_core_t));

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
        core->rl[LA64_REGISTER_CR2] = LA64_EXCEPTION_BAD_ACCESS;
        return;
    }

    /* preparing bitwalker */
    bitwalker_t bw;
    bitwalker_init_read(&bw, iptr, 256, BW_LITTLE_ENDIAN);

    /* getting opcode */
    core->op.op = (uint8_t)bitwalker_read(&bw, 8);

    uint8_t maxargs = opcode_maxargs[core->op.op];

    /* parsing loop */
    bool reached_end = false;
    for(uint8_t i = 0; i < maxargs && !reached_end; i++)
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
            {
                uint8_t rcnt = (uint8_t)bitwalker_read(&bw, 5);

                if(rcnt > LA64_REGISTER_RR &&
                   core->rl[LA64_REGISTER_CR0] < LA64_ELEVATION_KERNEL)
                {
                    core->rl[LA64_REGISTER_CR2] = LA64_EXCEPTION_BAD_INSTRUCTION;
                    return;
                }

                core->op.param[core->op.param_cnt] = &(core->rl[rcnt]);
                core->op.param_cnt++;

                break;
            }
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
                core->rl[LA64_REGISTER_CR2] = LA64_EXCEPTION_BAD_INSTRUCTION;
                reached_end = true;
                return;
        }
    }

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

    /* going into da execution loop */
    while(1)
    {
        if(!core->in_interrupt)
        {
            /* checking if exception is non-NONE */
            if(core->rl[LA64_REGISTER_CR2] != LA64_EXCEPTION_NONE)
            {
                core->halted = true;
                la64_raise_interrupt(core->machine, LA64_IRQ_EXCEPTION);
            }
            
             /* checking if core is halted */
            if(core->halted)
            {
                /* yield cpu to not burn it */
                usleep(100);
                goto skip_execution;
            }
        }

        /* decoding instruction */
        la64_core_decode_instruction_at_pc(core);

        /* sanity check */
        if((core->rl[LA64_REGISTER_CR2] != LA64_EXCEPTION_NONE ||
            core->op.op > LA64_OPCODE_MAX ||
            opfunc_table[core->op.op] == NULL) &&
           !core->in_interrupt)
        {
            core->rl[LA64_REGISTER_CR2] = LA64_EXCEPTION_BAD_INSTRUCTION;
            continue;
        }

        /* executing instruction */
        opfunc_table[core->op.op](core);

        /* incrementing program counter by instruction size */
        core->rl[LA64_REGISTER_PC] += core->op.ilen;

        /*
         * if we are in a interrupt then there is no reason
         * to check if we shall serve another interrupt.
         * if we dont check if the instruction executes was
         * the return from interrupt controller then there is
         * a potential for a hardware occuring TOCTOU vulnerability,
         * because we would just immediately interrupt into another
         * interrupt handler in the interrupt vector table.
         */
        if(core->in_interrupt ||
           core->op.op == LA64_OPCODE_IRET)
        {
            goto tick_timer;
        }

        /* interrupt controller checking routine starts here */
skip_execution:

        /* serve interrupt for the interrupt controller */
        la64_serve_interrupt_if_needed(core);

        /* tick the timer always */
    tick_timer:
        {
            la64_timer_tick(core->machine->timer, la64_get_host_cycles());
        }
    }

    return NULL;
}


void la64_core_execute(la64_core_t *core)
{
    /* sanity check */
    if(core == NULL ||
       core->pthread != 0)
    {
        return;
    }

    /* invoking execution */
    pthread_create(&(core->pthread), NULL, la64_core_execute_thread, (void*)core);

#if defined(__APPLE__)
    CFRunLoopRun();
#endif /* __APPLE__ */
    
    pthread_join(core->pthread, NULL);
}

void la64_core_terminate(la64_core_t *core)
{
    /* sanity check */
    if(core == NULL ||
       core->pthread == 0)
    {
        return;
    }
    
    if(pthread_self() == core->pthread)
    {
        pthread_exit(NULL);
    }
    
    pthread_cancel(core->pthread);
}
