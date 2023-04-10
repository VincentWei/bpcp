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

void bubbleSort(const char *a[], size_t n)
{
    size_t i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (strcmp(a[j], a[j+1]) > 0) {
                const char *t = a[j];
                a[j] = a[j+1];
                a[j+1] = t;
            }
        }
    }
}

int main (void)
{
    int i;
    const char *s[] = {
        "zhang",
        "gou",
        "xu",
        "zheng",
        "mao",
        "zhao",
        "li",
        "bai",
        "qing"
    };

    for (i = 0; i < 8; i++) {
        printf("%s  ", s[i]);
    }
    puts(s[8]);

    bubbleSort (s, 8);

    for (i = 0; i < 8; i++) {
        printf ("%s  ", s[i]);
    }
    puts(s[8]);

    return 0;
}

