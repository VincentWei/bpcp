#include <stdio.h>

int main(void)
{
    char s1[200], s2[100];

    scanf("%s%s", s1, s2);

    printf ("The first string got: \"%s\"\n", s1);
    printf ("The second string got: \"%s\"\n", s2);

    int i, j;

    /* get the length of the first string */
    for (i = 0; s1[i]; i++);

    for (j = 0; s2[j]; j++) {
        s1[i++] = s2[j];
    }
    s1[i] = '\0';

    printf ("The string concatenated: \"%s\"\n", s1);
    return 0;
}

