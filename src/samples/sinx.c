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
#include <math.h>
#include <assert.h>

static int sn(int n)
{
    int m;

    assert(n >= 0);

    if (n == 0 || n == 1)
        m = 1;
    else
        m = n * sn(n - 1);

    return m;
}

static double sx(int n, double x)
{
    double m;

    assert(n >= 0);

    if (n == 0)
        m = 1.0;
    else
        m = x * sx(n - 1, x);

    return m;
}

int main(void)
{
    int m = 1, f = 1, i = 1, t = 1;
    double s = 0.0, x;
    double a;

    printf("Please input x: \n");
    scanf("%lf", &x);

    a = f * sx(m, x) / sn(m);

    while (fabs(a) >= 1e-7 && t < 20) {
        t++;

        s += a;
        f = -f;

        i++;
        m = 2 * i - 1;
        a = f * sx(m, x) / sn(m);
    }

    printf("sinx(%lf): %lf\n", x, s);
    return 0;
}


