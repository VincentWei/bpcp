#include <stdio.h>

int main(void)
{

    int i, j;
    char t, a[20];

    for (i = 0; i < 20; i++)
        a[i] = getchar();


    for (j = 0; j < 19; j++) {

        for (i = 0; i < 19 - j; i++) {

            if (a[i] < a[i+1]) {
                t = a[i];
                a[i] = a[i+1];
                a[i+1] = t;
            }
        }
    }

    for (i = 0; i < 20; i++)
        printf("%c", a[i]);

    return 0;
}
