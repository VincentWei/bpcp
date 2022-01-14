#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define DEFSZ_FACTORS   4

unsigned int *prime_factors(unsigned int natural, size_t *nr_factors)
{
    unsigned int *factors = NULL;
    size_t sz = DEFSZ_FACTORS;

    assert(nr_factors);

    if (natural < 3) {
        goto failed;
    }

    factors = malloc(sz * sizeof(unsigned int));
    if (factors == NULL)
        goto failed;

    *nr_factors = 0;
    for (unsigned int u = 2; u <= natural; u++) {
        if (natural % u == 0) {
            do {
                natural = natural / u;

            } while (natural % u == 0);

            if (*nr_factors >= sz) {
                sz += DEFSZ_FACTORS;
                factors = realloc(factors, sizeof(unsigned int) * sz);
                if (factors == NULL)
                    goto failed;
            }

            factors[*nr_factors] = u;
            *nr_factors = *nr_factors + 1;
        }
    }

    return factors;

failed:
    if (factors)
        free(factors);
    *nr_factors = 0;
    return NULL;
}

int main(void)
{
    unsigned int natural;
    int n;

    do {
        printf("Please input an unsigned integer greater than 2:\n");

        errno = 0;
        n = scanf("%u", &natural);

        if (n != 1) {
            int ch;
            do {
                ch = getchar();
            } while (ch != '\n');
        }
    } while (errno || n != 1 || natural < 3);

    unsigned int *factors;
    size_t nr_factors;

    printf("Calculating the prime factor(s) of %u...\n", natural);
    factors = prime_factors(natural, &nr_factors);

    if (factors) {

        printf("The prime factor(s) of %u (totally %lu):\n",
                natural, nr_factors);
        for (size_t i = 0; i < nr_factors; i++) {
            printf("%u", factors[i]);
            if (i < nr_factors - 1) {
                putchar(',');
                putchar(' ');
            }
        }
        putchar('\n');

        /* DONOT forget to free factors */
        free(factors);
    }
    else {
        printf("Failed to get the prime factor(s) of %u\n", natural);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

