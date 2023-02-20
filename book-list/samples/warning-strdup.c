#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    char *str = strdup("Foo");
    printf("Duplicated string: %s\n", str);
    free(str);
    return 0;
}

