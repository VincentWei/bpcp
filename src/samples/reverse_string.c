#include <stdio.h>

static void reverse_string(char *array, size_t n)
{
    size_t i;
    char t;

    for (i = 0; i < n/2; i++) {
        size_t j = n - 1 - i;

        t = array[i];
        array[i] = array[j];
        array[j] = t;
    }
}

static const char *reverse_string_alt(char *array, size_t n)
{
    size_t i;
    char t;

    for (i = 0; i < n/2; i++) {
        size_t j = n - 1 - i;

        t = array[i];
        array[i] = array[j];
        array[j] = t;
    }

    return array;
}

#define NR_CHARS    5

int main (void)
{
    char x[NR_CHARS + 1];
    int i;

    printf("Please input %d characters:\n", NR_CHARS);
    for(i = 0; i < NR_CHARS; i++) {
        scanf("%c", x + i);
    }

    reverse_string(x, NR_CHARS);

    printf("Reversed: ");
    for(i = 0; i < NR_CHARS; i++) {
        putchar(x[i]);
    }
    putchar('\n');

    /* make sure the string is NULL terminated if you want to use "%s" */
    x[NR_CHARS] = '\0';
    printf("Reversed: %s\n", x);

    printf("Original: %s\n", reverse_string_alt(x, NR_CHARS));

    return 0;
}
