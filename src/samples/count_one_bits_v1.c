#include <stdio.h>
#include <stdlib.h>

int count_one_bits(unsigned char byte)
{
    int n = 0;

    for (int i = 0; i < 8; i++) {
        if ((byte >> i) & 0x01) {
            n++;
        }
    }

    return n;
}

int main(void)
{
    int byte;

    printf("static int nr_one_bits[] = {\n");
    for (byte = 0; byte <= 0xFF; byte++) {
        printf("    %d,\n", count_one_bits((unsigned char)byte));
    }
    printf("};\n");

    return 0;
}

