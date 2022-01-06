#include <stdio.h>

size_t replace_char (char *s, char needle)
{
    size_t i = 0, n = 0;
    char ch;

    while (s[i]) {

        if (s[i] == needle) {

            for (ch = 'a'; ch <= 'z'; ch++) {

                if ((i > 0 && s[i - 1] == ch) ||
                        (s[i] != '\0' && s[i + 1] == ch)) {
                    continue;
                }

                s[i] = ch;
                n++;
                break;
            }
        }

        i++;
    }

    return n;
}

int main(void)
{
    char buf[16];

    while (1) {
        printf ("Please input a string (less than 16 characters)"
                " and press <ENTER>, <Ctrl+D> to quit\n");

        if (fgets(buf, sizeof(buf), stdin) == NULL || *buf == 0) {
            break;
        }

        size_t n = replace_char (buf, '?');
        if (n > 0) {
            printf ("Replaced %lu chars: %s\n", n, buf);
        }
        else {
            printf ("The string is not modified: %s\n", buf);
        }
    }

    return 0;
}
