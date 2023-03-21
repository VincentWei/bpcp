#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    int n = 3;
    bool b = 3;
    printf("The size of bool: %u, the value of bool: %d\n",
            (unsigned)sizeof(b), (int)b);

    printf("(n > 1) == 2 is %s\n", ((n > 1) == 2) ? "true" : "false");
    printf("(n > 1) == (2) is %s\n", ((n > 1) == (2)) ? "true" : "false");
    printf("(n > 1) == !!2 is %s\n", ((n > 1) == !!2) ? "true" : "false");
    return 0;
}
