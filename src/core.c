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

#include <la64/core.h>
#include <la64/register.h>
#include <la64/memory.h>
#include <la64/machine.h>
#include <la64/instruction/core.h>

#include <lautils/bitwalker.h>

la64_opfunc_t opfunc_table[LA64_OPCODE_MAX + 1] = {
    /* core operations */
    la64_op_hlt,
    la64_op_nop,
};

la64_core_t *la64_core_alloc()
{
    /* allocate a brand new core */
    la64_core_t *core = calloc(1, sizeof(la64_core_t));

    /* allocate registers */
    for(unsigned char i = 0b0000; i < (LA64_REGISTER_MAX + 1); i++)
    {
        core->rl[i] = la64_register_alloc();
    }

    /* put the special registers in nicely packaged shortcuts */
    core->pc = core->rl[LA64_REGISTER_PC];
    core->sp = core->rl[LA64_REGISTER_SP];
    core->fp = core->rl[LA64_REGISTER_FP];
    core->cf = core->rl[LA64_REGISTER_CF];

    return core;
}

void la64_core_dealloc(la64_core_t *core)
{
    /* release all registers */
    for(unsigned char i = 0b00000000; i < 0b00001100; i++)
    {
        la64_register_dealloc(core->rl[i]);
    }

    /* release core */
    free(core);
}

static void la64_core_decode_instruction_at_pc(la64_core_t *core)
{
    /* reset operation structure */
    //memset(&(core->op), 0, sizeof(la16_operation_t));

    /* preparing real address for memory */
    //unsigned short pc_real_addr = *(core->pc);

    /* using la16 memory page protection access */
    /*if(!la16_mpp_access(core, &pc_real_addr, LA16_PAGEU_FLAG_EXEC, 2))
    {*/
        /* setting operation to halt */
        /*core->op.op = LA16_OPCODE_HLT;*/

        /* setting termination flag to bad access */
        /*core->term = LA16_TERM_FLAG_BAD_ACCESS;
        return;
    }*/

    /* initilize */
    /*bitwalker_t bw;
    bitwalker_init_read(&bw, &(core->machine->memory->memory[pc_real_addr]), sizeof(unsigned int), BW_LITTLE_ENDIAN);
    */
    /* setting opcode according to instruction */
    /*core->op.op = (uint8_t)bitwalker_read(&bw, 8);
    */
    /* extracting mode */
    /*unsigned char mode = (uint8_t)bitwalker_read(&bw, 3);
    */
    /* setting parameter to intermediate */
    /*core->op.param[0] = &(core->op.imm[0]);
    core->op.param[1] = &(core->op.imm[1]);
    */
    /* handling parameter mode */
    /*switch(mode)
    {
        case LA16_PARAMETER_CODING_COMBINATION_REG:
        {
            core->op.reg[0] = (uint8_t)bitwalker_read(&bw, 5);
            core->op.param[0] = core->rl[core->op.reg[0]];
            goto out_res_a_check;
        }
        case LA16_PARAMETER_CODING_COMBINATION_REG_REG:
        {
            core->op.reg[0] = (uint8_t)bitwalker_read(&bw, 5);
            core->op.reg[1] = (uint8_t)bitwalker_read(&bw, 5);
            core->op.param[0] = core->rl[core->op.reg[0]];
            core->op.param[1] = core->rl[core->op.reg[1]];
            goto out_res_a_check;
        }
        case LA16_PARAMETER_CODING_COMBINATION_IMM16:
        {
            core->op.imm[0] = (uint16_t)bitwalker_read(&bw, 16);
            break;
        }
        case LA16_PARAMETER_CODING_COMBINATION_IMM16_REG:
        {
            core->op.imm[0] = (uint16_t)bitwalker_read(&bw, 16);
            core->op.reg[0] = (uint8_t)bitwalker_read(&bw, 5);
            core->op.param[1] = core->rl[core->op.reg[0]];
            goto out_res_a_check;
        }
        case LA16_PARAMETER_CODING_COMBINATION_REG_IMM16:
        {
            core->op.reg[0] = (uint8_t)bitwalker_read(&bw, 5);
            core->op.param[0] = core->rl[core->op.reg[0]];
            core->op.imm[1] = (uint16_t)bitwalker_read(&bw, 16);
            goto out_res_a_check;
        }
        case LA16_PARAMETER_CODING_COMBINATION_IMM8_IMM8:
        {
            core->op.imm[0] = (uint8_t)bitwalker_read(&bw, 8);
            core->op.imm[1] = (uint8_t)bitwalker_read(&bw, 8);
            break;
        }
        default:
            break;
    }

    return;

out_res_a_check:
    // Find out what res contains
    if(core->op.reg[0] > LA16_REGISTER_EL0_MAX ||
       core->op.reg[1] > LA16_REGISTER_EL0_MAX)
    {
        if(*(core->el) == LA16_CORE_MODE_EL0)
        {
            core->op.op = LA16_OPCODE_HLT;
            core->term = LA16_TERM_FLAG_PERMISSION;
        }
    }*/
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

    // Set runs flag
    core->runs = 0b00000001;
    core->term = LA64_TERM_FLAG_NONE;

    while(1)
    {
        switch(core->term)
        {
            case LA64_TERM_FLAG_NONE:
                break;
            case LA64_TERM_FLAG_HALT:
                printf("[exec] halted at 0x%llx\n", *(core->pc));
                core->runs = 0b00000000;
                return NULL;
            case LA64_TERM_FLAG_BAD_ACCESS:
                printf("[exec] bad access at 0x%llx\n", *(core->pc));
                core->runs = 0b00000000;
                return NULL;
            case LA64_TERM_FLAG_PERMISSION:
                printf("[exec] permission denied at 0x%llx\n", *(core->pc));
                core->runs = 0b00000000;
                return NULL;
            default:
                printf("[exec] unknown exception at 0x%llx\n", *(core->pc));
                break;
        }

        la64_core_decode_instruction_at_pc(core);

        if(core->op.op <= LA64_OPCODE_MAX && opfunc_table[core->op.op] != NULL)
        {
            opfunc_table[core->op.op](core);
        }
        else
        {
            printf("[exec] illegal opcode: 0x%x\n", core->op.op);
            opfunc_table[LA64_OPCODE_HLT](core);
        }

        *(core->pc) += 4;
    }

    core->runs = 0b00000000;
    return NULL;
}


void la64_core_execute(la64_core_t *core)
{
    /* core running check */
    if(core->runs)
    {
        return;
    }

    /* invoking execution */
    pthread_t pthread;
    pthread_create(&pthread, NULL, la64_core_execute_thread, (void*)core);
    pthread_join(pthread, NULL);
}

void la64_core_terminate(la64_core_t *core)
{
    /* setting termination flag (TODO: needs atomics) */
    core->term = 0b00000001;
}
