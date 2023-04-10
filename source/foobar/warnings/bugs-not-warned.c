/*
 * @file bugs-not-warned.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/29
 * @brief Bugs would not warned by compiler.
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

static unsigned int number_of_calls(unsigned int *nr_calls)
{
    static unsigned int _nr_calls;

    _nr_calls++;
    *nr_calls = _nr_calls;

    return _nr_calls;
}

int main(void)
{
    unsigned int nr_calls;

    number_of_calls(&nr_calls);
    number_of_calls(&nr_calls);
    printf("The number of calls: %u\n", nr_calls);

    const char fmt_a[] = "%s";
    printf(fmt_a, 123);   // checked

    const char *fmt_b = "%s";
    printf(fmt_b, 456);   // not checked
    return 0;
}

