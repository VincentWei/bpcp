#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
   The prototype of qsort_r():

    void qsort_r(void *base, size_t nmemb, size_t size,
        int (*compar)(const void *, const void *, void *), void *arg);
*/

/* the compare callback for qsort_r */
static int my_compare(const void * p1, const void * p2, void *arg)
{
    double a, b;

    a = *(const double *)p1;
    b = *(const double *)p2;

    if (a > b)
        return 1;
    else if (a < b)
        return -1;
    return 0;
}

int main(void)
{
    static double floats[] = { M_PI, M_E, -M_PI, -M_E, 0, 1, -1 };
    static const size_t nr_floats = sizeof(floats)/sizeof(floats[0]);

    qsort_r(floats, nr_floats, sizeof(double), my_compare, NULL);

    printf("The sorted floats:\n");
    for (size_t i = 0; i < nr_floats; i++) {
        printf("\t%f\n", floats[i]);
    }

    return 0;
}

