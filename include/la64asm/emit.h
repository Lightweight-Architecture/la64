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

#ifndef LA16_EMIT_H
#define LA16_EMIT_H

#include <la64vm/core.h>
#include <la64vm/memory.h>
#include <la64asm/type.h>
#include <la64asm/label.h>
#include <la64asm/opcode.h>
#include <stdbool.h>

/* opcode emit */
void la64_compiler_emit_opcode(bitwalker_t *bw, uint8_t op);

/* register emit */
void la64_compiler_emit_reg(bitwalker_t *bw, uint8_t reg);

/* intermediate emit */
void la64_compiler_emit_imm8(bitwalker_t *bw, uint8_t imm);
void la64_compiler_emit_imm16(bitwalker_t *bw, uint16_t imm);
void la64_compiler_emit_imm32(bitwalker_t *bw, uint32_t imm);
void la64_compiler_emit_imm64(bitwalker_t *bw, uint64_t imm);
void la64_compiler_emit_imm(bitwalker_t *bw, uint64_t imm);

/* end emitter */
void la64_compiler_emit_end(bitwalker_t *bw);

/* instruction emitter */
bool la64_compiler_emit_instr_inc(const opcode_entry_t *opce, compiler_line_t *cl, bitwalker_t *bw);
bool la64_compiler_emit_instr_dec(const opcode_entry_t *opce, compiler_line_t *cl, bitwalker_t *bw);
bool la64_compiler_emit_instr_clr(const opcode_entry_t *opce, compiler_line_t *cl, bitwalker_t *bw);
bool la64_compiler_emit_instr_default(const opcode_entry_t *opce, compiler_line_t *cl, bitwalker_t *bw);

/* automised code emitting */
bool la64_compiler_emit(compiler_line_t *cl);
bool la64_compiler_emit_all(compiler_invocation_t *ci);

#endif /* LA16_EMIT_H */
