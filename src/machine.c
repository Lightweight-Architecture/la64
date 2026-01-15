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
#include <la64/machine.h>

#include <la64/device/rtc.h>

la64_machine_t *la64_machine_alloc(uint64_t memory_size)
{
    /* allocating brand new machine */
    la64_machine_t *machine = calloc(1, sizeof(la64_machine_t));

    /* null pointer check */
    if(machine == NULL)
    {
        return NULL;
    }

    /* allocate memory */
    machine->memory = la64_memory_alloc(memory_size);

    /* null pointer check */
    if(machine->memory == NULL)
    {
        /* release machine and return nothing */
        goto out_release_machine;
    }

    /* allocate mmio */
    machine->mmio_bus = la64_mmio_alloc();

    /* null pointer check */
    if(machine->mmio_bus == NULL)
    {
        /* release machine, memory and return nothing */
        goto out_release_memory;
    }

    /* allocate new core */
    machine->core = la64_core_alloc();

    /* null pointer check */
    if(machine->core == NULL)
    {
        goto out_release_mmio;
    }

    machine->core->machine = machine;

    /* allocate interrupt controller */
    machine->intc = la64_intc_alloc(machine->core);

    /* null pointer check */
    if(machine->intc == NULL)
    {
        goto out_release_core;
    }

    /* register interrupt controller MMIO */
    if(!la64_mmio_register(machine->mmio_bus, LA64_INTC_BASE, LA64_INTC_SIZE, machine->intc, la64_intc_read, la64_intc_write, "intc"))
    {
        goto out_release_intc;
    }

    /* allocate timer */
    machine->timer = la64_timer_alloc(LA64_TIMER_FREQ,  LA64_IRQ_TIMER);

    /* null pointer check */
    if(machine->timer == NULL)
    {
        goto out_release_intc;
    }

    /* register timer MMIO */
    if(!la64_mmio_register(machine->mmio_bus, LA64_TIMER_BASE, LA64_TIMER_SIZE, machine->timer, la64_timer_read, la64_timer_write, "timer"))
    {
        goto out_release_timer;
    }

    /* allocate uart */
    machine->uart = la64_uart_alloc(machine->core, LA64_IRQ_UART);

    if(machine->uart == NULL)
    {
        goto out_release_timer;
    }

    la64_uart_start(machine->uart);

    if(!la64_mmio_register(machine->mmio_bus, LA64_UART_BASE, LA64_UART_SIZE, machine->uart, la64_uart_read, la64_uart_write, "uart"))
    {
        goto out_release_uart;
    }

    /* register rtc */
    if(!la64_mmio_register(machine->mmio_bus, LA64_RTC_BASE, LA64_RTC_SIZE, NULL, la64_rtc_read, la64_rtc_write, "rtc"))
    {
        goto out_release_uart;
    }

    if(!la64_mmio_register(machine->mmio_bus, LA64_MC_BASE, LA64_MC_SIZE, NULL, la64_mc_read, la64_mc_write, "mc"))
    {
        goto out_release_uart;
    }

    /* allocate platform */
    machine->platform = la64_platform_alloc(machine->core);

    if(machine->platform == NULL)
    {
        goto out_release_uart;
    }

    if(!la64_mmio_register(machine->mmio_bus, LA64_PLATFORM_BASE, LA64_PLATFORM_SIZE, machine->platform, la64_platform_read, la64_platform_write, "platform"))
    {
        goto out_release_platform;
    }

#if defined(__linux__)
    machine->display = la64_display_alloc();

    if(machine->display == NULL)
    {
        goto out_release_display;
    }

    if(!la64_mmio_register(machine->mmio_bus, LA64_FB_BASE, LA64_FB_SIZE, machine->display, la64_fb_read, la64_fb_write, "fb"))
    {
        goto out_release_platform;
    }
#endif /* __linux__ */

    return machine;

    /* much more compact error handling */
#if defined(__linux__)
out_release_display:
    la64_display_dealloc(machine->display);
#endif /* __linux__ */
out_release_platform:
    la64_platform_dealloc(machine->platform);
out_release_uart:
    la64_uart_dealloc(machine->uart);
out_release_timer:
    la64_timer_dealloc(machine->timer);
out_release_intc:
    la64_intc_dealloc(machine->intc);
out_release_core:
    la64_core_dealloc(machine->core);
out_release_mmio:
    la64_mmio_dealloc(machine->mmio_bus);
out_release_memory:
    la64_memory_dealloc(machine->memory);
out_release_machine:
    free(machine);
    return NULL;
}

void la64_machine_dealloc(la64_machine_t *machine)
{
    /* null pointer check */
    if(machine == NULL)
    {
        return;
    }

#if defined(__linux__)
    /* release display */
    if(machine->display)
    {
        la64_display_dealloc(machine->display);
    }
#endif /* __linux__ */

    /* release platform */
    if(machine->platform)
    {
        la64_platform_dealloc(machine->platform);
    }

    /* release uart */
    if(machine->uart)
    {
        la64_uart_stop(machine->uart);
        la64_uart_dealloc(machine->uart);
    }

    /* release timer */
    if(machine->timer)
    {
        la64_timer_dealloc(machine->timer);
    }

    /* release interrupt controller */
    if(machine->intc)
    {
        la64_intc_dealloc(machine->intc);
    }

    /* release cores */
    la64_core_dealloc(machine->core);

    /* release mmio */
    if(machine->mmio_bus)
    {
        la64_mmio_dealloc(machine->mmio_bus);
    }

    /* release memory */
    la64_memory_dealloc(machine->memory);

    /* release machine */
    free(machine);
}