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


