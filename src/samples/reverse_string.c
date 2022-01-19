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

#include <stdio.h>

static void reverse_string(char *array, size_t n)
{
    size_t i;
    char t;

    for (i = 0; i < n/2; i++) {
        size_t j = n - 1 - i;

        t = array[i];
        array[i] = array[j];
        array[j] = t;
    }
}

static const char *reverse_string_alt(char *array, size_t n)
{
    size_t i;
    char t;

    for (i = 0; i < n/2; i++) {
        size_t j = n - 1 - i;

        t = array[i];
        array[i] = array[j];
        array[j] = t;
    }

    return array;
}

#define NR_CHARS    5

int main (void)
{
    char x[NR_CHARS + 1];
    int i;

    printf("Please input %d characters:\n", NR_CHARS);
    for(i = 0; i < NR_CHARS; i++) {
        scanf("%c", x + i);
    }

    reverse_string(x, NR_CHARS);

    printf("Reversed: ");
    for(i = 0; i < NR_CHARS; i++) {
        putchar(x[i]);
    }
    putchar('\n');

    /* make sure the string is NULL terminated if you want to use "%s" */
    x[NR_CHARS] = '\0';
    printf("Reversed: %s\n", x);

    printf("Original: %s\n", reverse_string_alt(x, NR_CHARS));

    return 0;
}
