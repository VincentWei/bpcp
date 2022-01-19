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

size_t replace_char (char *s, char needle)
{
    size_t i = 0, n = 0;
    char ch;

    while (s[i]) {

        if (s[i] == needle) {

            for (ch = 'a'; ch <= 'z'; ch++) {

                if ((i > 0 && s[i - 1] == ch) ||
                        (s[i] != '\0' && s[i + 1] == ch)) {
                    continue;
                }

                s[i] = ch;
                n++;
                break;
            }
        }

        i++;
    }

    return n;
}

int main(void)
{
    char buf[16];

    while (1) {
        printf ("Please input a string (less than 16 characters)"
                " and press <ENTER>, <Ctrl+D> to quit\n");

        if (fgets(buf, sizeof(buf), stdin) == NULL || *buf == 0) {
            break;
        }

        size_t n = replace_char (buf, '?');
        if (n > 0) {
            printf ("Replaced %lu chars: %s\n", n, buf);
        }
        else {
            printf ("The string is not modified: %s\n", buf);
        }
    }

    return 0;
}
