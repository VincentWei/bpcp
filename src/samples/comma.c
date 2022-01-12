#include <stdio.h>
#include <errno.h>

int main(void)
{
    int a, b, c, max;

    do {
        errno = 0;
        printf("Please input three integers (-2147483648～2147483647)\n");
        scanf("%d,%d,%d", &a, &b, &c);
    } while (errno == ERANGE);

    if (a > b && a > c)
        max = a;
    else if (b > c)
        max = b;
    else if (c > b)
        max = c;

    printf("The max in (%d, %d, %d) is %d\n", a, b, c, max);
    return 0;
}

