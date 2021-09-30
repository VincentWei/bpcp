/*
 * @file format.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/30
 * @brief Show format warnings.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of FooBar, which contains the examples of my course:
 * _the Best Practices of C Language_.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

static void foo(const char *format, int a, int b)
{
    char buf [8];

    /* -Wformat-overflow */
    sprintf(buf, "a = %i, b = %i\n", a, b);

    /* -Wformat-security */
    printf(format);

    /* -Wformat-truncation */
    snprintf(buf, sizeof(buf), "a = %i, b = %i\n", a, b);
}

int format(const char *name)
{
    char buff[4];

    int i = 0;
    unsigned int u = 0;

    long int li = 0;
    unsigned long int uli = 0;

    long long int lli = 0;
    unsigned long long int ulli = 0;

    printf("Hello, %s!", name);

    /* warns with -Wformat */
    printf("The int: %d for %s!", name);

    /* warns with -Wformat-zero-length */
    printf("", name);

    /* warns with -format-contains-nul */
    printf("Hello, \0 %s:", name);

    /* warns with -Wformat-extra-args */
    printf("Hello, %s!", name, i);

    sprintf(buff, "a = %i, b = %i\n", i, u);
    return i;
}

