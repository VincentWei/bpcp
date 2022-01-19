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
#include <ctype.h>

int main(void)
{
    int c;

    while (1) {
        unsigned char uc;

        c = getchar();
        uc = (unsigned char)c;

        if (isdigit(c)) {
            int v = c - 48;
            printf("Got a digit character: %d\n", v);
        }
        else if (isupper(c)) {
            printf("Got a upper case letter: %c\n", c);
        }
        else if (islower(c)) {
            printf("Got a lower case letter: %c\n", c);
        }
        else if (c == EOF) {
            printf("Got EOF\n");
            break;
        }
        else if (uc >= 0x20 && uc < 0x80) {
            printf("Got a non-digit-and-alpha printable character: %c\n", c);
        }
        else if (uc < 0x20) {
            printf("Got a control character: 0x%x\n", c);
        }
        else {
            printf("Got a non-ASCII character: 0x%x\n", c);
        }
    }

    return 0;
}
