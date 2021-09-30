/*
 * @file lexical.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/30
 * @brief Show lexical warnings.
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

static int foo (int n)
{
    /* warns with -Wvla */
    int a[n];

    for (int i = 0; i < n; i++) {
        a[i] = 0;
    }

    switch (a[0]) {
    case 0:
        a[1] = 1;
        break;

    case 1:
        a[1] = 5;

    /* -Wimplicit-fallthrough */
    case 2:
        a[1] = 9;
        break;
    }

    return a[0];
}

static size_t bar (char a[])
{
    size_t sz;

    if (a[0])
        sz = sizeof("A string literal");
    else
        sz = sizeof(a);

    return sz;
}

struct s { int f, g, h; };

/* warns with -Wmissing-field-initializers */
static struct s _x = { 3, 4 };

static struct s _y;

/* warned with -Wpadded */
struct X {
    char a;
    int b;
    int c;
};

int lexical(const char *name)
{
    int i;
    int n = 5;

    (void)name;

    /* warns with -Wduplicated-cond */
    if (n > 0) {
        i = 0;
    }
    else if (n > 0) {
        i = 1;
    }

    /* warns with -Wduplicated-branches */
    if (n > 0) {
        i = 0;
    }
    else {
        i = 0;
    }

    /* warns with -Wempty-body */
    if (n > 0) {
    }

    char a[10];

    /* -Wdangling-else */
    if (n > 0)
        if (i > 0)
            foo (10);
    else
        bar (a);

    /* warns with -Warrayn-bounds and -O2 */
    a[11] = 0;

    i++;

    printf("_x.f: %d, _y.h: %d\n", _x.f, _y.h);

    bad ();

    /* warned with -Wpacked */
    struct foo {
        int x;
        char a, b, c, d;
    } __attribute__((packed));

    struct bar {
        char z;
        struct foo f;
    };

    return 0;
}

