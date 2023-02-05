#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __USE_POSIX
static char *strdup(const char *s)
{
    char *dup = malloc(strlen(s) + 1);
    strcpy(dup, s);
    printf("Using our own implementation of strdup()\n");
    return dup;
}
#endif /* !__USE_POSIX */

static void foo(const char *str)
{
#ifndef __USE_GNU
    char *my_str = strdup(str);
    printf("using strdup(): ");
#else
    char *my_str = strdupa(str);
    printf("using strdupa(): ");
#endif

    /* do something using my_str */
    printf("%s\n", my_str);

#ifndef __USE_GNU
    free(my_str);
#endif
}

int main(void)
{
    foo("hello");
    return 0;
}

