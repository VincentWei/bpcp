/*
 * @file typesafe.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/30
 * @brief Show type safe warnings.
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

static void foo(const char *string)
{
    /* -Wdiscarded-qualifiers */
    char *a_str = "A string lietral";

    a_str[0] = 'A';

    /* -Wcast-qual (not enabled by -Wall) */
    char *my_str = (char *)string;

    puts(my_str);
    puts(a_str);

    char buff[10];
    int *a_int_p;
    /* -Wcast-align (not enabled by -Wall) */
    a_int_p = (int *)(buff + 1);

    *a_int_p = 10;
}

static void *bar(void *data)
{
    unsigned int n = 5;
    bool b;

    /* -Wtype-limits */
    b = (n >= 0);

    /* -Wbool-operation */
    b++;

    /* -Wpointer-arith */
    data++;

    float x = 0.001f;
    double y = 0.001;
    /* -Wfloat-equal (not enabled by -Wall and -Wextra) */
    if (x == y) {
        return NULL;
    }

    /* -Wbool-compare */
    if ((n > 1) == 2)
        return NULL;

    (void)data;
    return (void *)0;
}

static bool foobar(int foo, unsigned int bar)
{
    double x = 0.01;
    char c = 0;

    /* warns with -Wfloat-conversion */
    int i = x;

    /* warns with -Wconversion */
    c = c + 1;

    /* warns with -Wconversion */
    c = c + foo;

    /* suppress unused warning */
    i++;

    /* -Wsign-compare */
    return bar > foo;
}

typedef int (*cb_foo) (void* data);

int typesafe(void)
{
    void *p;
    char buff[100];
    int i;

    /* -Wcast-function-type */
    cb_foo fn;
    fn = (cb_foo)foo;
    int retv = fn(buff);

    /* -Wint-to-pointer-cast */
    p = (void *)fn(buff);

    /* -Wincompatible-pointer-types */
    fn = bar;

    /* -Wpointer-to-int-cast */
    i = (int)buff;

    foo("Hello");

    /* suppress unused warning */
    p = p;
    i++;

    foobar(-1, 1);

    return retv;
}

