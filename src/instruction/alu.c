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
#include <la64/instruction/alu.h>

#define DEFINE_LA64_ARITHMETIC_OP(act)                                                              \
    if(core->op.param_cnt == 2)                                                                     \
    {                                                                                               \
        *(core->op.param[0]) = *(core->op.param[0]) act *(core->op.param[1]);                       \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        *(core->op.param[0]) = *(core->op.param[1]) act *(core->op.param[2]);                       \
    }

#define DEFINE_LA64_SIGNED_ARITHMETIC_OP(act)                                                       \
    if(core->op.param_cnt == 2)                                                                     \
    {                                                                                               \
        *(core->op.param[0]) = (int64_t)*(core->op.param[0]) act (int64_t)*(core->op.param[1]);     \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        *(core->op.param[0]) = (int64_t)*(core->op.param[1]) act (int64_t)*(core->op.param[2]);     \
    }

#define DEFINE_LA64_ARITHMETIC_OP_ZERO_BAD(act)                                                     \
    uint64_t *operand[2];                                                                           \
    if(core->op.param_cnt == 2)                                                                     \
    {                                                                                               \
        operand[0] = core->op.param[0];                                                             \
        operand[1] = core->op.param[1];                                                             \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        operand[0] = core->op.param[1];                                                             \
        operand[1] = core->op.param[2];                                                             \
    }                                                                                               \
    if(*operand[1] == 0)                                                                            \
    {                                                                                               \
        core->exception = LA64_EXCEPTION_BAD_ARITHMETIC;                                            \
        return;                                                                                     \
    }                                                                                               \
    *(core->op.param[0]) = *operand[0] act *operand[1];

#define DEFINE_LA64_SIGNED_ARITHMETIC_OP_ZERO_BAD(act)                                              \
    uint64_t *operand[2];                                                                           \
    if(core->op.param_cnt == 2)                                                                     \
    {                                                                                               \
        operand[0] = core->op.param[0];                                                             \
        operand[1] = core->op.param[1];                                                             \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        operand[0] = core->op.param[1];                                                             \
        operand[1] = core->op.param[2];                                                             \
    }                                                                                               \
    if(*operand[1] == 0)                                                                            \
    {                                                                                               \
        core->exception = LA64_EXCEPTION_BAD_ARITHMETIC;                                            \
        return;                                                                                     \
    }                                                                                               \
    *(core->op.param[0]) = (int64_t)*operand[0] act (int64_t)*operand[1];

void la64_op_add(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(+);
}

void la64_op_sub(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(-);
}

void la64_op_mul(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(*);
}

void la64_op_div(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP_ZERO_BAD(/);
}

void la64_op_idiv(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_SIGNED_ARITHMETIC_OP_ZERO_BAD(/);
}

void la64_op_mod(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP_ZERO_BAD(%);
}

void la64_op_inc(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt == 0);

    for(uint8_t i = 0; i < core->op.param_cnt; i++)
    {
        (*(core->op.param[i]))++;
    }
}

void la64_op_dec(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt == 0);

    for(uint8_t i = 0; i < core->op.param_cnt; i++)
    {
        (*(core->op.param[i]))--;
    }
}

void la64_op_not(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);

    *(core->op.param[0]) = ~*(core->op.param[0]); 
}

void la64_op_and(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(&);
}

void la64_op_or(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(|);
}

void la64_op_xor(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(^);
}

void la64_op_shr(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(>>);
}

void la64_op_shl(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    DEFINE_LA64_ARITHMETIC_OP(<<);
}

void la64_op_ror(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);

    uint64_t v = *core->op.param[0];
    uint64_t n = (core->op.param_cnt == 2) ? (*core->op.param[1] & 63) : 1;
    *core->op.param[0] = (v >> n) | (v << (64 - n));
}

void la64_op_rol(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 1);

    int64_t v = *core->op.param[0];
    uint64_t n = (core->op.param_cnt == 2) ? (*core->op.param[1] & 63) : 1;
    *core->op.param[0] = (v << n) | (v >> (64 - n));
}

void la64_op_pdep(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    uint64_t src, mask;
    uint64_t *dest;

    if(core->op.param_cnt == 2)
    {
        dest = core->op.param[0];
        src = *core->op.param[0];
        mask = *core->op.param[1];
    }
    else
    {
        dest = core->op.param[0];
        src = *core->op.param[1];
        mask = *core->op.param[2];
    }

#ifdef __BMI2__
    *dest = _pdep_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t src_bit = 0;

    for(int i = 0; i < 64; i++)
    {
        if(mask & (1ULL << i))
        {
            if(src & (1ULL << src_bit))
            {
                result |= (1ULL << i);
            }
            src_bit++;
        }
    }

    *dest = result;
#endif
}

void la64_op_pext(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2 && core->op.param_cnt != 3);

    uint64_t src, mask;
    uint64_t *dest;

    if(core->op.param_cnt == 2)
    {
        dest = core->op.param[0];
        src = *core->op.param[0];
        mask = *core->op.param[1];
    }
    else
    {
        dest = core->op.param[0];
        src = *core->op.param[1];
        mask = *core->op.param[2];
    }

#ifdef __BMI2__
    *dest = _pext_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t dest_bit = 0;

    for(int i = 0; i < 64; i++)
    {
        if(mask & (1ULL << i))
        {
            if(src & (1ULL << i))
            {
                result |= (1ULL << dest_bit);
            }
            dest_bit++;
        }
    }

    *dest = result;
#endif
}