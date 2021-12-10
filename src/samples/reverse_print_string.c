#include <stdio.h>
#include <string.h>
#include <assert.h>

size_t print_string(const char *str)
{
    /* return the length of the string for convenience */
    size_t n = 0;

    /* check the '\0' character */
    while (*str) {
        putchar(*str);

        str++;
        n++;
    }

    return n;
}

size_t reverse_print_string(const char *str)
{
    size_t n = strlen(str);

    if (n >= 1) {
        size_t i = n - 1;

        do {
            putchar(str[i]);

            if (i == 0)
                break;
            i--;
        } while (1);
    }

    return n;
}

#define SZ_BUFF 64

int main(void)
{
    /* No need to initialize the buffer */
    char str[SZ_BUFF];

    printf("Please input a string:\n");

    /* Never use gets(), use fgets() instead */
    fgets(str, sizeof(str), stdin);

    putchar('\n');

    printf("The reversed string:\n");
    size_t n = reverse_print_string(str);

    putchar('\n');
    printf("Totally %lu characters (including `\\n` character) in the string.\n", n);

    return 0;
}

