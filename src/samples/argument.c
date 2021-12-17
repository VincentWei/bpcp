#include <stdio.h>

int main(void)
{
    int a = 0, b;

    b = (a = 5);

    printf("%d %d\n", a=5, a++);
    printf("%d %d\n", b==6, a);
    printf("%d %d\n", !(b=0), a);

    return 0;
}
