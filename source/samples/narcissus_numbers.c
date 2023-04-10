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
#include <stdbool.h>

static bool is_narcissus(unsigned candidant, int n)
{
    unsigned int t = candidant;
    unsigned int sum = 0;

    do {
        unsigned int d = t % 10;
        unsigned power = d;

        int j = 1;
        while (j < n) {
            power *= d;
            j++;
        }

        sum += power;
        t /= 10;
    } while (t > 0);

    if (sum == candidant) {
        return true;
    }

    return false;
}

int main(void)
{
    int n;

    scanf("%d", &n);
    if (n < 3) {
        printf("N (%d) must be greater than 2!\n", n);
        return 1;
    }

    unsigned int first = 1;
    for (int i = 1; i < n; i++) {
        first *= 10;
    }

    /* just try a known narcissus to check the implementation */
    if (!is_narcissus(153, 3)) {
        printf("Bad implementation!\n");
        return 1;
    }

    printf("We start from %u...\n", first);

    unsigned int found = 0;
    unsigned int candidant;
    for (candidant = first; candidant < first * 10; candidant++) {
        if (is_narcissus(candidant, n)) {
            printf("Found one: %u\n", candidant);
            found++;
        }
    }
    printf("Totally %u found\n", found);

    return 0;
}
