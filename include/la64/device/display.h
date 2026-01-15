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

#if defined(__linux__) || defined(__APPLE__)

#ifndef LA64_DEVICE_DISPLAY_H
#define LA64_DEVICE_DISPLAY_H

#define LA64_FB_WIDTH   256
#define LA64_FB_HEIGHT  256
#define LA64_FB_TICK_HZ 64.0
#define LA64_FB_TICK_DT 1.0 / TICK_HZ

#define LA64_FB_REG_ENABLED 0x00
#define LA64_FB_PALLETE     0x01
#define LA64_FB_FRAMEBUFFER 0x301

#define LA64_FB_BASE        0x1FE00700
#define LA64_FB_SIZE        LA64_FB_FRAMEBUFFER + (LA64_FB_WIDTH * LA64_FB_HEIGHT)

#include <stdatomic.h>
#include <stdint.h>
#include <pthread.h>

typedef struct la64_core la64_core_t;

typedef struct {
    uint8_t enabled;
    uint8_t *palette;
    uint8_t *fb;
    pthread_t pthread;
} la64_display_t;

la64_display_t *la64_display_alloc(void);
void la64_display_dealloc(la64_display_t *display);

void *display_start(void *arg);

uint64_t la64_fb_read(la64_core_t *core, void *device, uint64_t offset, int size);
void la64_fb_write(la64_core_t *core, void *device, uint64_t offset, uint64_t value, int size);

#endif /* LA64_DEVICE_DISPLAY_H */

#endif /* __linux__ */