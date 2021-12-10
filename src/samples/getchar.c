#include <stdio.h>
#include <ctype.h>

int main(void)
{
    int c;

    while (1) {
        unsigned char uc;

        c = getchar();
        uc = (unsigned char)c;

        if (isdigit(c)) {
            int v = c - 48;
            printf("Got a digit character: %d\n", v);
        }
        else if (isupper(c)) {
            printf("Got a upper case letter: %c\n", c);
        }
        else if (islower(c)) {
            printf("Got a lower case letter: %c\n", c);
        }
        else if (c == EOF) {
            printf("Got EOF\n");
            break;
        }
        else if (uc >= 0x20 && uc < 0x80) {
            printf("Got a non-digit-and-alpha printable character: %c\n", c);
        }
        else if (uc < 0x20) {
            printf("Got a control character: 0x%x\n", c);
        }
        else {
            printf("Got a non-ASCII character: 0x%x\n", c);
        }
    }

    return 0;
}
