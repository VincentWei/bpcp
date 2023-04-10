/*
 * Copyright (C) 2022 Vincent Wei (<https://github.com/VincentWei/>)
 *
 * This is a part of Vincent's online course:
 * [the Best Practices of C Language](https://courses.fmsoft.cn/best-practices-of-c/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

static void access_out_of_range(unsigned int range)
{
    char *buff1, *buff2;

    buff1 = calloc(sizeof(char),  4);
    buff2 = calloc(sizeof(char),  4);

    printf("Going to set %d bytes to `$` in buff1 (%p)\n", range, buff1);
    memset(buff1, '$', range);

    printf("Going to set %d bytes to `^` in buff2 (%p)\n", range, buff2);
    memset(buff2, '^', range);

    puts("Content in buff1");
    for (int i = 0; i < 4; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 4; i++) {
        putchar(buff2[i]);
    }
    puts("");

    free(buff1);
    free(buff2);
}

int main(void)
{
    unsigned int range = 8;

    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

