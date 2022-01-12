#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void)
{
    char *author, *lang;
    int n;

    while (1) {
        puts("Please input author and language separated by a white space"
             " like `Vincent ZH` or <Ctrl+D> to quit:");

        errno = 0;
        n = scanf("%ms%ms", &author, &lang);
        if (n == 2) {

            // TODO: do something with author and language here
            printf("The author got: \"%s\"\n", author);
            printf("The language got: \"%s\"\n", lang);

            free(author);
            free(lang);
        }
        else if (n == EOF) {
            break;
        }
        else if (errno != 0) {
            perror("Failed scanf");
            break;
        }
        else {
            puts("Bad input\n");
        }
    }

    return EXIT_SUCCESS;
}

