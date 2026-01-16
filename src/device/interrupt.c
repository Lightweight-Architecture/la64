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
#include <stdio.h>
#include <string.h>

#include <la64/device/interrupt.h>
#include <la64/core.h>
#include <la64/machine.h>
#include <la64/memory.h>

la64_intc_t *la64_intc_alloc(la64_core_t *core)
{
    /* allocate interrupt controller */
    la64_intc_t *intc = calloc(1, sizeof(la64_intc_t));

    /* null pointer check */
    if(intc == NULL)
    {
        return NULL;
    }

    /* setup interrupt controller */
    intc->core = core;
    intc->current_irq = -1;
    intc->enabled = 0;
    intc->ctrl = 0;
    intc->vector_base = 0;

    return intc;
}

void la64_intc_dealloc(la64_intc_t *intc)
{
    /* null pointer check */
    if(intc == NULL)
    {
        return;
    }

    free(intc);
}

void la64_raise_interrupt(la64_core_t *core,
                          int irq_line)
{
    /* sanity checks */
    if(core == NULL ||
       core->machine == NULL ||
       core->machine->intc == NULL ||
       irq_line < 0 ||
       irq_line > LA64_IRQ_MAX)
    {
        return;
    }
    
    /* setting pending bit for intc */
    core->machine->intc->pending |= (1ULL << irq_line);
}

void la64_clear_interrupt(la64_core_t *core, int irq_line)
{
    /* sanity checks */
    if(core == NULL ||
       core->machine == NULL ||
       core->machine->intc == NULL ||
       irq_line < 0 ||
       irq_line > LA64_IRQ_MAX)
    {
        return;
    }
    
    /* clear pending bit */
    core->machine->intc->pending &= ~(1ULL << irq_line);
}

static int find_pending_irq(la64_intc_t *intc)
{
    /* get pending and enabled interrupts */
    uint64_t active = intc->pending & intc->enabled;

    /* checking if interrupts are enabled */
    if(active == 0)
    {
        return -1;
    }
    
    /* iterating for lowest set bit */
    for(int i = 0; i <= LA64_IRQ_MAX; i++)
    {
        if(active & (1ULL << i))
        {
            return i;
        }
    }
    
    return -1;
}

bool la64_intc_check(la64_core_t *core)
{
    /* sanity checks */
    if(core == NULL ||
       core->machine == NULL ||
       core->machine->intc == NULL)
    {
        return false;
    }
    
    /* check if interrupts are globally enabled */
    if(!(core->machine->intc->ctrl & LA64_INTC_CTRL_ENABLE))
    {
        return false;
    }
    
    /* check if were already servicing an interrupt (unless nesting allowed) */
    if(core->machine->intc->current_irq >= 0 &&
       !(core->machine->intc->ctrl & LA64_INTC_CTRL_NESTING))
    {
        return false;
    }
    
    /* find highest priority pending interrupt */
    int irq = find_pending_irq(core->machine->intc);
    if(irq < 0)
    {
        return false;
    }
    
    /* save current PC */
    core->machine->intc->saved_pc = core->rl[LA64_REGISTER_PC];
    
    /* mark which IRQ were servicing */
    core->machine->intc->current_irq = irq;
    
    /* clear pending bit (edge-triggered style) */
    core->machine->intc->pending &= ~(1ULL << irq);

    uint64_t vector_addr = core->machine->intc->vector_base + (irq * 8);
    
    /* read handler address from vector table */
    void *vector_ptr = la64_memory_access(core, vector_addr, 8);
    if(vector_ptr == NULL)
    {
        core->machine->intc->current_irq = -1;
        return false;
    }
    
    uint64_t handler_addr = *(uint64_t *)vector_ptr;
    
    /* jump to handler */
    core->rl[LA64_REGISTER_PC] = handler_addr;
    core->op.ilen = 0;
    
    return true;
}

bool la64_intc_pending(la64_intc_t *intc)
{
    if(intc == NULL)
    {
        return false;
    }
    
    /* check if interrupts are globally enabled */
    if(!(intc->ctrl & LA64_INTC_CTRL_ENABLE))
    {
        return false;
    }
    
    /* check if were already servicing (and nesting disabled) */
    if(intc->current_irq >= 0 && !(intc->ctrl & LA64_INTC_CTRL_NESTING))
    {
        return false;
    }
    
    /* check for any pending and enabled interrupt */
    return (intc->pending & intc->enabled) != 0;
}

uint64_t la64_intc_read(la64_core_t *core, void *device, uint64_t offset, int size)
{
    la64_intc_t *intc = (la64_intc_t *)device;

    /* null pointer check */
    if(intc == NULL)
    {
        return 0;
    }
    
    switch(offset)
    {
        case LA64_INTC_REG_PENDING:
            return intc->pending; 
        case LA64_INTC_REG_ENABLED:
            return intc->enabled;
        case LA64_INTC_REG_CTRL:
            return intc->ctrl;
        case LA64_INTC_REG_VECTOR:
            return intc->vector_base;
        case LA64_INTC_REG_CURRENT:
            return (uint64_t)intc->current_irq;
        case LA64_INTC_REG_SAVED_PC:
            return intc->saved_pc;
        default:
            return 0;
    }
}

void la64_intc_write(la64_core_t *core, void *device, uint64_t offset, uint64_t value, int size)
{
    la64_intc_t *intc = (la64_intc_t *)device;

    /* null pointer check */
    if(intc == NULL)
    {
        return;
    }
    
    switch (offset) {
        case LA64_INTC_REG_PENDING:
            intc->pending &= ~value;
            break;
        case LA64_INTC_REG_ENABLED:
            intc->enabled = value;
            break;
        case LA64_INTC_REG_CTRL:
            intc->ctrl = value;
            break;
        case LA64_INTC_REG_VECTOR:
            intc->vector_base = value;
            break;
        case LA64_INTC_REG_ACK:
            if((int64_t)value == intc->current_irq)
            {
                intc->current_irq = -1;
            }
            break;
        case LA64_INTC_REG_SAVED_PC:
            break;
        default:
            break;
    }
}
