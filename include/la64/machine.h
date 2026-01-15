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

#ifndef LA64_MACHINE_H
#define LA64_MACHINE_H

#include <la64/core.h>
#include <la64/memory.h>
#include <la64/mmio.h>

#include <la64/device/timer.h>
#include <la64/device/interrupt.h>
#include <la64/device/uart.h>
#include <la64/device/mc.h>
#include <la64/device/platform.h>

#if defined(__linux__)
#include <la64/device/display.h>
#endif /* __linux__ */

#include <stdint.h>

typedef struct la64_machine {
    la64_core_t *core;
    la64_memory_t *memory;
    la64_mmio_bus_t *mmio_bus;
    la64_intc_t *intc;
    la64_timer_t *timer;
    la64_uart_t *uart;
    la64_platform_t *platform;
#if defined(__linux__)
    la64_display_t *display;
#endif /* __linux__ */
} la64_machine_t;

la64_machine_t *la64_machine_alloc(uint64_t memory_size);
void la64_machine_dealloc(la64_machine_t *machine);

#endif /* LA64_MACHINE_H */