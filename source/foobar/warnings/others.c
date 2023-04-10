/*
 * @file others.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/30
 * @brief Show other warnings.
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

#include <stdlib.h>
#include <string.h>

int attrs(const char *p) __attribute__ ((__nonnull__ (1)));

int attrs(const char *p)
{
    if (p == NULL)
        return -1;

    char buff[8];
    memcpy(buff, NULL, sizeof(buff));
    return 0;
}

int others(void)
{
    float x = 0.001f;
    double y;

    /* -Wabsolute-value */
    /* -Wbuiltin-declaration-mismatch */
    y = abs(x);

    y += 1.0;

    return 0;
}

