#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *foo(int v0, int v1)
{
    char *buf;
    asprintf(&buf, "the value is %d", v0, v1);
    return buf;
}

int main(void)
{
    printf("__USE_FORTIFY_LEVEL: %d; sizeof(bool): %d\n",
            __USE_FORTIFY_LEVEL, (int)sizeof(bool));
    char *str = foo(1, 2);
    puts(str);
    free(str);
    return 0;
}
