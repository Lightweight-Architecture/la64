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
#include <unistd.h>
#include <la64asm/compiler.h>
#include <la64asm/diag.h>

int main(int argc, char *argv[])
{
    int opt;
    const char *output_path = NULL;

    /* parse options */
    while((opt = getopt(argc, argv, "o:")) != -1)
    {
        switch (opt)
        {
            case 'o':
                output_path = optarg;
                break;
            default:
                break;
        }
    }

    if(!output_path)
    {
        diag_warn(NULL, "no output binary specified, falling back to a.out\n");
        output_path = "a.out";
    }

    /* remaining arguments are input files */
    int file_count = argc - optind;
    if(file_count <= 0)
    {
        diag_error(NULL, "no input files provided\n");
    }

    /* handling input files */
    char **files = calloc(file_count, sizeof(char *));
    if(!files)
    {
        diag_error(NULL, "fatal\n");
    }

    /* copying their paths over */
    for(int i = 0; i < file_count; i++)
    {
        files[i] = strdup(argv[optind + i]);
        if(!files[i])
        {
            diag_error(NULL, "fatal\n");
            return 1;
        }
    }

    /* compile */
    compile_files((const char **)files, file_count, output_path);

    /* cleanup */
    for(int i = 0; i < file_count; i++)
    {
        free(files[i]);
    }
    free(files);

    return 0;
}
