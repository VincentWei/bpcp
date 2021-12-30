#include <stdio.h>

int main(void)
{
    int s = 0, i = 1;

    while (s <= 1000) {
        s += i;
        i++;
    }

    printf("s = %d\n", s);
    return 0;
}

