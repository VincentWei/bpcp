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
#include <stdlib.h>
#include <errno.h>

int main(void)
{
    char *author, *lang;
    int n;

    while (1) {
        puts("Please input author and language separated by a white space"
             " like `Vincent ZH` or <Ctrl+D> to quit:");

        errno = 0;
        n = scanf("%ms%ms", &author, &lang);
        if (n == 2) {

            // TODO: do something with author and language here
            printf("The author got: \"%s\"\n", author);
            printf("The language got: \"%s\"\n", lang);

            free(author);
            free(lang);
        }
        else if (n == EOF) {
            break;
        }
        else if (errno != 0) {
            perror("Failed scanf");
            break;
        }
        else {
            puts("Bad input\n");
        }
    }

    return EXIT_SUCCESS;
}

