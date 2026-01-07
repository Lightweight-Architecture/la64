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

#ifndef LA64_DEVICE_INTERRUPT_H
#define LA64_DEVICE_INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct la64_core la64_core_t;

#define LA64_IRQ_TIMER      0
#define LA64_IRQ_UART       1
#define LA64_IRQ_DISK       2
#define LA64_IRQ_NETWORK    3
#define LA64_IRQ_SOFTWARE   4
/* IRQ 5-63 available for user devices */

#define LA64_IRQ_MAX        63

#define LA64_INTC_REG_PENDING   0x00
#define LA64_INTC_REG_ENABLED   0x08
#define LA64_INTC_REG_CTRL      0x10
#define LA64_INTC_REG_VECTOR    0x18
#define LA64_INTC_REG_ACK       0x20
#define LA64_INTC_REG_CURRENT   0x28
#define LA64_INTC_REG_SAVED_PC  0x30

/* control register bits */
#define LA64_INTC_CTRL_ENABLE   (1 << 0)
#define LA64_INTC_CTRL_NESTING  (1 << 1)

typedef struct la64_intc {
    uint64_t pending;
    uint64_t enabled;
    uint64_t ctrl;
    uint64_t vector_base;
    int64_t  current_irq;
    uint64_t saved_pc;
    la64_core_t *core;
} la64_intc_t;

la64_intc_t *la64_intc_alloc(la64_core_t *core);
void la64_intc_dealloc(la64_intc_t *intc);

void la64_raise_interrupt(la64_core_t *core, int irq_line);
void la64_clear_interrupt(la64_core_t *core, int irq_line);
bool la64_intc_check(la64_core_t *core);
bool la64_intc_pending(la64_intc_t *intc);

uint64_t la64_intc_read(void *device, uint64_t offset, int size);
void la64_intc_write(void *device, uint64_t offset, uint64_t value, int size);

#endif /* LA64_DEVICE_INTERRUPT_H */