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
#include <string.h>
#include <assert.h>

size_t print_string(const char *str)
{
    /* return the length of the string for convenience */
    size_t n = 0;

    /* check the '\0' character */
    while (*str) {
        putchar(*str);

        str++;
        n++;
    }

    return n;
}

size_t reverse_print_string(const char *str)
{
    size_t n = strlen(str);

    if (n >= 1) {
        size_t i = n - 1;

        do {
            putchar(str[i]);

            if (i == 0)
                break;
            i--;
        } while (1);
    }

    return n;
}

#define SZ_BUFF 64

int main(void)
{
    /* No need to initialize the buffer */
    char str[SZ_BUFF];

    printf("Please input a string:\n");

    /* Never use gets(), use fgets() instead */
    fgets(str, sizeof(str), stdin);

    putchar('\n');

    printf("The reversed string:\n");
    size_t n = reverse_print_string(str);

    putchar('\n');
    printf("Totally %lu characters (including `\\n` character) in the string.\n", n);

    return 0;
}

