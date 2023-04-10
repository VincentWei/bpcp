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
#include <assert.h>

/* use qsort() if there are more integers */
static void sort_three_integers(int *numbers)
{
    int min, mid, max;
    int min_i = 0, max_i = 0, mid_i;

    min = numbers[0];
    max = numbers[0];

    if (numbers[1] < min) {
        min = numbers[1];
        min_i = 1;
    }
    else {
        max = numbers[1];
        max_i = 1;
    }

    if (numbers[2] < min) {
        min = numbers[2];
        min_i = 2;
    }
    else {
        max = numbers[2];
        max_i = 2;
    }

    assert(min_i != max_i);

    mid_i = 3 - min_i - max_i;
    mid = numbers[mid_i];

    numbers[0] = min;
    numbers[1] = mid;
    numbers[2] = max;
}

int main (void)
{
    int i;
    int numbers[3];

    for (i = 0; i < 3; i++) {
        scanf ("%d", numbers + i);
    }

    printf("Input integers: %d, %d, %d\n",
            numbers[0], numbers[1], numbers[2]);

    sort_three_integers(numbers);

    printf("Sorted integers: %d, %d, %d\n",
            numbers[0], numbers[1], numbers[2]);

    return 0;
}

