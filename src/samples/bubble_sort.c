#include <stdio.h>
#include <string.h>

void bubbleSort(const char *a[], size_t n)
{
    size_t i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (strcmp(a[j], a[j+1]) > 0) {
                const char *t = a[j];
                a[j] = a[j+1];
                a[j+1] = t;
            }
        }
    }
}

int main (void)
{
    int i;
    const char *s[] = {
        "zhang",
        "gou",
        "xu",
        "zheng",
        "mao",
        "zhao",
        "li",
        "bai",
        "qing"
    };

    for (i = 0; i < 8; i++) {
        printf("%s  ", s[i]);
    }
    puts(s[8]);

    bubbleSort (s, 8);

    for (i = 0; i < 8; i++) {
        printf ("%s  ", s[i]);
    }
    puts(s[8]);

    return 0;
}

