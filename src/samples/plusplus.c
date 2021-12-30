#include <stdio.h>

int main(void)
{
    int ch;

    ch = 'A';
    ch = ch++;
    printf("ch = ch++: %c\n", ch);

    ch = 'A';
    ch = ++ch;
    printf("ch = ++ch: %c\n", ch);

    ch = 'A';
    ch++;
    printf("ch++: %c\n", ch);

    int chB;

    ch = 'A';
    chB = ch++;
    printf("chB = ch++:  %c\n", chB);

    return 0;
}
