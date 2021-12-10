#include <stdio.h>

int main (void)
{
    char m[10];

#if 0
    for (int i = 0; i < 10; i++) {
        scanf ("%c", m + i);
    }
#endif

    scanf ("%c%c%c%c%c%c%c%c%c%c", m, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7, m + 8, m + 9);

    printf ("%c\n", m[0]);
    return 0;
}

