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

#include <lautils/bitwalker.h>

/* helper */

bw_endian_t bw_host_endian(void)
{
    static const uint16_t test = 0x0001;
    return (*((const uint8_t*)&test) == 0x01) ? BW_LITTLE_ENDIAN : BW_BIG_ENDIAN;
}

uint64_t bw_swap_n(uint64_t v,
                   uint8_t num_bytes)
{
    switch (num_bytes)
    {
        case 2:  return __builtin_bswap16((uint16_t)v);
        case 3:  return ((v >> 16) & 0xFF) | (v & 0xFF00) | ((v & 0xFF) << 16);
        case 4:  return __builtin_bswap32((uint32_t)v);
        case 5:
        case 6:
        case 7:
        case 8:  return __builtin_bswap64(v) >> ((8 - num_bytes) * 8);
        default: return v;
    }
}

/* initialization */

void bitwalker_init(bitwalker_t *bw,
                    uint8_t *buf,
                    size_t capacity,
                    bw_endian_t endian)
{
    bw->buffer   = buf;
    bw->bit_pos  = 0;
    bw->capacity = capacity;
    bw->endian   = endian;
    memset(buf, 0, capacity);
}

void bitwalker_init_read(bitwalker_t *bw,
                         const uint8_t *buf,
                         size_t len,
                         bw_endian_t endian)
{
    bw->buffer   = (uint8_t *)buf;
    bw->bit_pos  = 0;
    bw->capacity = len;
    bw->endian   = endian;
}

/* management */

void bitwalker_reset(bitwalker_t *bw)
{
    bw->bit_pos = 0;
}

size_t bitwalker_tell(const bitwalker_t *bw)
{
    return bw->bit_pos;
}

/* read and write */

int bitwalker_write(bitwalker_t *bw,
                    uint64_t value,
                    uint8_t num_bits)
{
    if(num_bits == 0 || num_bits > 64)
    {
        return -1;
    }
    else if((bw->bit_pos + num_bits + 7) / 8 > bw->capacity)
    {
        return -1;
    }

    uint64_t mask = (num_bits == 64) ? ~0ULL : ((1ULL << num_bits) - 1);
    value &= mask;

    /*
     * for multi-byte values we gonna have to convert host endian to target endian
     * in case they aint the same
     */
    if(num_bits > 8)
    {
        uint8_t num_bytes = (num_bits + 7) / 8;
        if(bw_host_endian() != bw->endian)
        {
            value = bw_swap_n(value, num_bytes);
        }
    }

    /* pack bits sequentially into buffer */
    for(uint8_t i = 0; i < num_bits; i++)
    {
        uint8_t bit = (value >> i) & 1;

        size_t  byte_idx = bw->bit_pos / 8;
        uint8_t bit_idx  = bw->bit_pos % 8;

        if(bit)
        {
            bw->buffer[byte_idx] |= (1 << bit_idx);
        }
        bw->bit_pos++;
    }

    return 0;
}

uint64_t bitwalker_read(bitwalker_t *bw,
                        uint8_t num_bits)
{
    if(num_bits == 0 || num_bits > 64)
    {
        return 0;
    }
    else if((bw->bit_pos + num_bits + 7) / 8 > bw->capacity)
    {
        return 0;
    }

    uint64_t value = 0;

    /* unpack bits sequentially from buffer */
    for(uint8_t i = 0; i < num_bits; i++)
    {
        size_t  byte_idx = bw->bit_pos / 8;
        uint8_t bit_idx  = bw->bit_pos % 8;
        uint8_t bit = (bw->buffer[byte_idx] >> bit_idx) & 1;

        if(bit)
        {
            value |= (1ULL << i);
        }
        bw->bit_pos++;
    }

    /*
     * for multi-byte values we gonna have to convert target endian to host endian
     * in case they aint the same
     */
    if(num_bits > 8)
    {
        uint8_t num_bytes = (num_bits + 7) / 8;
        if(bw_host_endian() != bw->endian)
        {
            value = bw_swap_n(value, num_bytes);
        }
    }

    return value;
}

uint64_t bitwalker_peek(bitwalker_t *bw,
                        uint8_t num_bits)
{
    size_t saved = bw->bit_pos;
    uint64_t result = bitwalker_read(bw, num_bits);
    bw->bit_pos = saved;
    return result;
}

void bitwalker_skip(bitwalker_t *bw,
                    size_t num_bits)
{
    bw->bit_pos += num_bits;
}

void bitwalker_seek(bitwalker_t *bw,
                    size_t bit_pos)
{
    bw->bit_pos = bit_pos;
}

size_t bitwalker_bytes_used(const bitwalker_t *bw)
{
    return (bw->bit_pos + 7) / 8;
}

void bitwalker_align_byte(bitwalker_t *bw)
{
    bw->bit_pos = (bw->bit_pos + 7) & ~(size_t)7;
}
