#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

static void foo(const char *format, int a, int b)
{
    char buf [8];

    /* -Wformat-overflow */
    sprintf(buf, "a = %d, b = %d\n", a, b);

    /* -Wformat-security */
    printf(format);

    /* -Wformat-truncation */
    snprintf(buf, sizeof(buf), "a = %d, b = %d\n", a, b);
}

int format(const char *name)
{
    char buff[4];

    int i = 0;
    unsigned int u = 0;

    long int li = 0;
    unsigned long int uli = 0;

    long long int lli = 0;
    unsigned long long int ulli = 0;

    printf("Hello, %s!", name);

    /* warns with -Wformat */
    printf("The int: %d for %s!", name);

    /* warns with -Wformat-zero-length */
    printf("", name);

    /* warns with -format-contains-nul */
    printf("Hello, \0 %s:", name);

    /* warns with -Wformat-extra-args */
    printf("Hello, %s!", name, i);

    sprintf(buff, "a = %d, b = %d\n", i, u);
    return i;
}

int main(void)
{
    int a = 2, b = 3;
    printf("a = %d, b = %f\n", a, b);
    return 0;
}

