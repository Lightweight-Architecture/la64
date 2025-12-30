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

#include <termios.h>
#include <unistd.h>

#include <la64/instruction/instruction.h>
#include <la64/instruction/io.h>

void la64_op_in(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    switch(*(core->op.param[1]))
    {
        case LA16_IO_PORT_SERIAL:
        {
            struct termios oldt, newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            read(STDIN_FILENO, core->op.param[0], 1);
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            break;
        }
        default:
            break;
    }
}

void la64_op_out(la64_core_t *core)
{
    la64_instr_termcond(core->op.param_cnt != 2);

    switch(*(core->op.param[0]))
    {
        case LA16_IO_PORT_SERIAL:
        {
            write(STDOUT_FILENO, core->op.param[1], 1);
            break;
        }
        default:
            break;
    }
}