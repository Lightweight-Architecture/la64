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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <la64asm/section.h>
#include <lautils/parser.h>
#include <lautils/bitwalker.h>
#include <la64asm/code.h>
#include <la64asm/diag.h>

void code_token_section(compiler_invocation_t *ci)
{
    /* iterating for section token type */
    for(unsigned long i = 0; i < ci->line_cnt; i++)
    {
        if(ci->line[i].type == COMPILER_LINE_TYPE_SECTION)
        {
            if(strcmp(ci->line[i].token[1].str, ".data") == 0)
            {
                /* iterating till section data is over */
                i++;
                for(; i < ci->line_cnt && ci->line[i].type == COMPILER_LINE_TYPE_SECTION_DATA; i++)
                {
                    /* checking count */
                    if(ci->line[i].token_cnt < 3)
                    {
                        diag_error(&(ci->line[i].token[ci->line[i].token_cnt - 1]), "insufficient tokens for entry in .data section\n");
                    }

                    /* inserting address as label */
                    ci->label[ci->label_cnt].name = strdup(ci->line[i].token[0].str);
                    ci->label[ci->label_cnt++].addr = ci->image_addr;

                    /* checking if its known */
                    int dbs = 8;
                    if(strcmp(ci->line[i].token[1].str, "dw") == 0)
                    {
                        dbs = 16;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "dd") == 0)
                    {
                        dbs = 32;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "dq") == 0)
                    {
                        dbs = 64;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "db") != 0)
                    {
                        diag_error(&(ci->line[i].token[1]), "invalid data type for .data section entry \"%s\"\n", ci->line[i].token[1].str);
                    }

                    /* iterating through the chain */
                    for(unsigned long a = 2; a < ci->line[i].token_cnt; a++)
                    {
                        /* using low level type parser */
                        parser_return_t pr = parse_value_from_string(ci->line[i].token[a].str);

                        /* checking type */
                        if(pr.type == laParserValueTypeBuffer)
                        {
                            /* its a buffer so we copy the buffer into section */
                            char *buffer = (char*)pr.value;
                            for(unsigned short j = 0; j < pr.len; j++)
                            {
                                ci->image[ci->image_addr + j] = (unsigned char)buffer[j];
                            }
                            ci->image_addr += pr.len;
                        }
                        else if(pr.type == laParserValueTypeString)
                        {
                            if(dbs != 64)
                            {
                                diag_error(&(ci->line[i].token[a]), "don't put labels inside improper data types, i watch you!\n");
                            }

                            /* using finally the relocation table to its full extend */
                            reloc_table_entry_t *rtbe = ci->rtbe;
                            while(rtbe != NULL &&
                                  rtbe->next != NULL)
                            {
                                rtbe = rtbe->next;
                            }

                            if(rtbe == NULL)
                            {
                                rtbe = calloc(1, sizeof(reloc_table_entry_t));
                                ci->rtbe = rtbe;
                            }
                            else
                            {
                                rtbe->next = calloc(1, sizeof(reloc_table_entry_t));
                                rtbe = rtbe->next;
                            }

                            rtbe->name = strdup(ci->line[i].token[a].str);
                            rtbe->ctlink = &(ci->line[i].token[a]);
                            bitwalker_init(&(rtbe->bw), &(ci->image[ci->image_addr]), 8, BW_LITTLE_ENDIAN);
                            ci->image_addr += 8;
                        }
                        else
                        {
                            bitwalker_t bw;

                            /* storing value */
                            bitwalker_init(&bw, &(ci->image[ci->image_addr]), dbs / 8, BW_LITTLE_ENDIAN);
                            bitwalker_write(&bw, pr.value, dbs);
                            ci->image_addr += bitwalker_bytes_used(&bw);
                        }
                    }
                }
                i--;
            }
            else if(strcmp(ci->line[i].token[1].str, ".bss") == 0)
            {
                /* finding variable type */
                i++;
                for(; i < ci->line_cnt && ci->line[i].type == COMPILER_LINE_TYPE_SECTION_DATA; i++)
                {
                    /* checking count */
                    if(ci->line[i].token_cnt != 3)
                    {
                        diag_error(&(ci->line[i].token[ci->line[i].token_cnt - 1]), "insufficient or too many tokens for entry in .bss section\n");
                    }

                    /* insert label into label array */
                    ci->label[ci->label_cnt].name = strdup(ci->line[i].token[0].str);
                    ci->label[ci->label_cnt++].addr = ci->image_addr;

                    /* find out size */
                    int dbs = 8;
                    if(strcmp(ci->line[i].token[1].str, "dw") == 0)
                    {
                        dbs = 16;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "dd") == 0)
                    {
                        dbs = 32;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "dq") == 0)
                    {
                        dbs = 64;
                    }
                    else if(strcmp(ci->line[i].token[1].str, "db") != 0)
                    {
                        diag_error(&(ci->line[i].token[1]), "invalid data type for .bss section entry \"%s\"\n", ci->line[i].token[1].str);
                    }

                    /* offset image address by value */
                    parser_return_t pr = parse_value_from_string(ci->line[i].token[2].str);

                    /* checking if the type makes sense */
                    /* imagine you read that comment and you realise that you had the same idea before */
                    if(pr.type != laParserValueTypeDecimal)
                    {
                        diag_error(&(ci->line[i].token[2]), "invalid size for .bss section entry \"%s\"\n", ci->line[i].token[2].str);
                    }

                    ci->image_addr += (dbs / 8) * pr.value;
                }
                i--;
            }
        }
    }
}