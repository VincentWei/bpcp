#include <stdio.h>
#include <stdlib.h>

static unsigned char nr_one_bits_half_byte[] = {
    0, 1, 1, 2, 1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4,
};

static inline int count_one_bits(unsigned char byte)
{
    return nr_one_bits_half_byte[byte & 0x0F] +
        nr_one_bits_half_byte[(byte & 0xF0) >> 4];
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

