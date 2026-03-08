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

#include <la64asm/compiler.h>
#include <la64asm/diag.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <lautils/parser.h>
#include <la64asm/opcode.h>
#include <la64asm/register.h>
#include <la64asm/label.h>

#include <lautils/bitwalker.h>

/* opcode emit */
void la64_compiler_emit_opcode(bitwalker_t *bw,
                               uint8_t op)
{
    bitwalker_write(bw, op, 8);
}

/* register emit */
void la64_compiler_emit_reg(bitwalker_t *bw,
                            uint8_t reg)
{
    assert(reg < LA64_REGISTER_MAX);

    bitwalker_write(bw, LA64_PARAMETER_CODING_REG, 3);
    bitwalker_write(bw, reg, 5);
}

/* intermediate emit */
void la64_compiler_emit_imm8(bitwalker_t *bw,
                             uint8_t imm)
{
    bitwalker_write(bw, LA64_PARAMETER_CODING_IMM8, 3);
    bitwalker_write(bw, imm, 8);
}

void la64_compiler_emit_imm16(bitwalker_t *bw,
                              uint16_t imm)
{
    bitwalker_write(bw, LA64_PARAMETER_CODING_IMM16, 3);
    bitwalker_write(bw, imm, 16);
}

void la64_compiler_emit_imm32(bitwalker_t *bw,
                              uint32_t imm)
{
    bitwalker_write(bw, LA64_PARAMETER_CODING_IMM32, 3);
    bitwalker_write(bw, imm, 32);
}

void la64_compiler_emit_imm64(bitwalker_t *bw,
                              uint64_t imm)
{
    bitwalker_write(bw, LA64_PARAMETER_CODING_IMM64, 3);
    bitwalker_write(bw, imm, 64);
}

void la64_compiler_emit_imm(bitwalker_t *bw,
                            uint64_t imm)
{
    if(imm <= 0xFF)
    {
        la64_compiler_emit_imm8(bw, (uint8_t)imm);
    }
    else if(imm <= 0xFFFF)
    {
        la64_compiler_emit_imm16(bw, (uint16_t)imm);
    }
    else if(imm <= 0xFFFFFFFF)
    {
        la64_compiler_emit_imm32(bw, (uint32_t)imm);
    }
    else if(imm <= 0xFFFFFFFFFFFFFFFF)
    {
        la64_compiler_emit_imm64(bw, (uint64_t)imm);
    }
}

/* end emitter */
void la64_compiler_emit_end(bitwalker_t *bw)
{
    bitwalker_write(bw, LA64_PARAMETER_CODING_INSTR_END, 3);
}

/* instruction emitter */
bool la64_compiler_emit_instr_inc(opcode_entry_t *opce,
                                  compiler_line_t *cl,
                                  bitwalker_t *bw)
{
    /*
     * background is this was a native instruction, but
     * was removed, because it was redundant, many
     * la64 programms tho use inc and thats why we create
     * a emit path for it that emits increment using
     * the addition opcode, it costs the same, just
     * 1 byte more for the end marker, plus.. nobody
     * actually used the multiargument feature of it.
     */
    
    for(uint64_t i = 1; i < cl->token_cnt; i++)
    {
        /* increment means each parameter, one opcode */
        la64_compiler_emit_opcode(bw, LA64_OPCODE_ADD);

        /* it must be a register */
        register_entry_t *reg = register_from_string(cl->token[i].str);

        if(reg == NULL)
        {
            diag_error(&(cl->token[i]), "expected register, got intermediate or label \"%s\"\n", cl->token[i].str);
            return false;
        }

        /* emit parameters */
        la64_compiler_emit_reg(bw, reg->reg);
        la64_compiler_emit_imm8(bw, 1);
        la64_compiler_emit_end(bw);
        bitwalker_align_byte(bw);
    }

    /* advancing image address */
    cl->ci->image_addr += bitwalker_bytes_used(bw);

    return true;
}

bool la64_compiler_emit_instr_dec(opcode_entry_t *opce,
                                  compiler_line_t *cl,
                                  bitwalker_t *bw)
{
    /*
     * background is this was a native instruction, but
     * was removed, because it was redundant, many
     * la64 programms tho use dec and thats why we create
     * a emit path for it that emits decrement using
     * the subtraction opcode, it costs the same, just
     * 1 byte more for the end marker, plus.. nobody
     * actually used the multiargument feature of it.
     */
    
    for(uint64_t i = 1; i < cl->token_cnt; i++)
    {
        /* increment means each parameter, one opcode */
        la64_compiler_emit_opcode(bw, LA64_OPCODE_SUB);

        /* it must be a register */
        register_entry_t *reg = register_from_string(cl->token[i].str);

        if(reg == NULL)
        {
            diag_error(&(cl->token[i]), "expected register, got intermediate or label \"%s\"\n", cl->token[i].str);
            return false;
        }

        /* emit parameters */
        la64_compiler_emit_reg(bw, reg->reg);
        la64_compiler_emit_imm8(bw, 1);
        la64_compiler_emit_end(bw);
        bitwalker_align_byte(bw);
    }

    /* advancing image address */
    cl->ci->image_addr += bitwalker_bytes_used(bw);

    return true;
}

bool la64_compiler_emit_instr_clr(opcode_entry_t *opce,
                                  compiler_line_t *cl,
                                  bitwalker_t *bw)
{
    /*
     * people would argue to emit XOR but XOR 
     * has a end marker while MOV doesnt, so
     * using MOV with 0 Is better for code density
     * than using XOR REG, REG... basically we will
     * emit...
     *
     * MOV REG, 0
     *
     * over
     *
     * XOR REG, REG, END
     *
     */
    
    for(uint64_t i = 1; i < cl->token_cnt; i++)
    {
        /* increment means each parameter, one opcode */
        la64_compiler_emit_opcode(bw, LA64_OPCODE_MOV);

        /* it must be a register */
        register_entry_t *reg = register_from_string(cl->token[i].str);

        if(reg == NULL)
        {
            diag_error(&(cl->token[i]), "expected register, got intermediate or label \"%s\"\n", cl->token[i].str);
            return false;
        }

        /* emit parameters */
        la64_compiler_emit_reg(bw, reg->reg);
        la64_compiler_emit_imm8(bw, 0);
        bitwalker_align_byte(bw);
    }

    /* advancing image address */
    cl->ci->image_addr += bitwalker_bytes_used(bw);

    return true;
}

bool la64_compiler_emit_instr_default(const opcode_entry_t *opce,
                                      compiler_line_t *cl,
                                      bitwalker_t *bw)
{
    /* setting opcode from entry */ 
    la64_compiler_emit_opcode(bw, opce->opcode);

    /* parse parameters */
    for(uint64_t i = 1; i < cl->token_cnt; i++)
    {
        /* getting rule for current argument */
        bool reg_only = opcode_arg_accepts_reg_only(opce,  i - 1);

        /* checking for register */
        register_entry_t *reg = register_from_string(cl->token[i].str);

        if(reg != NULL)
        {
            la64_compiler_emit_reg(bw, reg->reg);
            continue;
        }

        /* checking if allowed to be something else than a register */
        if(reg_only)
        {
            diag_error(&(cl->token[i]), "expected register, got intermediate or label \"%s\"\n", cl->token[i].str);
            return false;
        }

        /* parsing value */
        parser_return_t pr = parse_value_from_string(cl->token[i].str);

        if(pr.type == laParserValueTypeString)
        {
            /* its a label */
            /* set mode to 64bit, because a label is 64bit wide */
            bitwalker_write(bw, LA64_PARAMETER_CODING_IMM64, 3);

            /* it must be a label and therefore a entry in the new relocation table ;) */
            /* checking label type in question */
            char *label = NULL;

            /* checking for local label */
            if(cl->token[i].str[0] == '.')
            {
                asprintf(&label, "%s%s", cl->ci->label_scope, cl->token[i].str);
            }
            else
            {
                label = strdup(cl->token[i].str);
            }

            cl->ci->rtlb[cl->ci->rtlb_cnt].name = label;
            cl->ci->rtlb[cl->ci->rtlb_cnt].bw = *bw;
            cl->ci->rtlb[cl->ci->rtlb_cnt++].ctlink = &(cl->token[i]);

            /*
             * skip the 64bit the label occupies
             * as we added it to the relocation table
             * already. the relocation table later will
             * fill this space with the address.
             */
            bitwalker_skip(bw, 64);
        }
        else
        {
            /* its a intermediate */
            la64_compiler_emit_imm(bw, pr.value);
        }
    }

    if(opce->maxargs == 32 || opce->maxargs != (cl->token_cnt - 1))
    {
        la64_compiler_emit_end(bw);
    }

    cl->ci->image_addr += bitwalker_bytes_used(bw);

    return true;
}

/* automised code emitting */
bool la64_compiler_emit(compiler_line_t *cl)
{
    /* parameter count check */
    if(cl->token_cnt <= 0)
    {
        diag_error(&(cl->token[0]), "insufficient operands\n");
        return false;
    }
    else if(cl->token_cnt > 32)
    {
        diag_error(&(cl->token[0]), "holy smokes, why soo many operands, maximum is 32 operands in 64bit lightweight architecture\n");
        return false;
    }

    /* getting opcode entry if it exists */
    const opcode_entry_t *opce = opcode_from_string(cl->token[0].str);

    if(opce == NULL)
    {
        diag_error(&(cl->token[0]), "illegal opcode \"%s\"\n", cl->token[0].str);
        return false;
    }

    /* checking for deprecation */
    if(opce->dnstr != NULL)
    {
        diag_warn(&(cl->token[cl->token_cnt - 1]), "opcode \"%s\" is deprecated: %s\n", opce->name, opce->dnstr);
    }

    /* checking argument count */
    if((cl->token_cnt - 1) > opce->maxargs)
    {
        diag_error(&(cl->token[cl->token_cnt - 1]), "too many operands for opcode \"%s\", expected %d operands, but got %d operands\n", opce->name, opce->maxargs, cl->token_cnt - 1);
        return false;
    }
    else if((cl->token_cnt - 1) < opce->minargs)
    {
        diag_error(&(cl->token[cl->token_cnt - 1]), "too few operands for opcode \"%s\", expected %d operands, but got %d operands\n", opce->name, opce->minargs, cl->token_cnt - 1);
        return false;
    }

    /* initilizing bitwalker */
    bitwalker_t bw;
    bitwalker_init(&bw, &(cl->ci->image[cl->ci->image_addr]), 256, BW_LITTLE_ENDIAN);

    /* check if emitting pseudo instruction */
    assert(opce->handler != NULL);

    return opce->handler(opce, cl, &bw);
}

bool la64_compiler_emit_all(compiler_invocation_t *ci)
{
    /* iterate through each token */
    for(uint64_t i = 0; i < ci->line_cnt; i++)
    {
        /* checking for label */
        if(ci->line[i].type == COMPILER_LINE_TYPE_GLOBAL_LABEL ||
           ci->line[i].type == COMPILER_LINE_TYPE_LOCAL_LABEL)
        {
            /* insert into labels */
            code_token_label_append(&(ci->line[i].token[0]));
        }
        else if(ci->line[i].type == COMPILER_LINE_TYPE_ASM)
        {
            if(!la64_compiler_emit(&(ci->line[i])))
            {
                return false;
            }
        }
    }

    /*
     * appending binary end label, which is a compiler
     * constant.
     */
    ci->label[ci->label_cnt].addr = ci->image_addr;
    ci->label[ci->label_cnt++].name = strdup("__la64_exec_img_end");

    /*
     * the main code emitter appended labels the code
     * requires to the relocation table, so we have to
     * look each request up in the label lookup table
     * and insert each label at the place where a label
     * shall be.
     */
    for(uint64_t i = 0; i < ci->rtlb_cnt; i++)
    {
        uint64_t addr = label_lookup(ci, ci->rtlb[i].name);

        if(addr != COMPILER_LABEL_NOT_FOUND)
        {
            bitwalker_write(&(ci->rtlb[i].bw), addr, 64);
        }
        else
        {
            diag_error(ci->rtlb[i].ctlink, "label \"%s\" not found\n", ci->rtlb[i].name);
            return false;
        }
    }

    return true;
}
