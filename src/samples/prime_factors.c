/*
 * Copyright (C) 2022 Vincent Wei (<https://github.com/VincentWei/>)
 *
 * This is a part of Vincent's online course:
 * [the Best Practices of C Language](https://courses.fmsoft.cn/best-practices-of-c/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

#ifndef NDEBUG

#define SZ_TABLE(array)     (sizeof(array)/sizeof(array[0]))

static unsigned int primes_under_20[] = {
    2, 3, 5, 7, 11, 13, 17, 19,
};

struct prime_factors {
    unsigned int  natural;
    unsigned int  nr_factors;
    unsigned int  factors[SZ_TABLE(primes_under_20)];
};

#define DEF_SIZE    10

#define EXPAND_SPACE            \
do {                            \
    if (nr >= *sz) {            \
        *sz += DEF_SIZE;        \
        cases = realloc(cases, sizeof(struct prime_factors) * *sz); \
        assert(cases != NULL);  \
    }                           \
} while(0)

static struct prime_factors *generate_test_cases(size_t *sz)
{
    size_t nr = 0;
    struct prime_factors *cases = NULL;

    assert(sz != NULL);

    /* the test cases for the prime numbers themselves */
    *sz = 0;
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {

        EXPAND_SPACE;

        cases[nr].natural = primes_under_20[i];
        cases[nr].nr_factors = 1;
        cases[nr].factors[0] = primes_under_20[i];

        nr++;
    }

    /* the test case for the product of all prime numbers under 20 */
    EXPAND_SPACE;

    cases[nr].natural = 1;
    for (size_t i; i < SZ_TABLE(primes_under_20); i++) {
        cases[nr].natural *= primes_under_20[i];
    }
    cases[nr].nr_factors = SZ_TABLE(primes_under_20);
    for (size_t i; i < SZ_TABLE(primes_under_20); i++) {
        cases[nr].factors[i] = primes_under_20[i];
    }
    nr++;

    /* the test cases of the squres of the prime numbers under 20 */
    for (size_t i; i < SZ_TABLE(primes_under_20); i++) {

        EXPAND_SPACE;

        cases[nr].natural = primes_under_20[i] * primes_under_20[i];
        cases[nr].nr_factors = 2;
        cases[nr].factors[0] = primes_under_20[i];
        cases[nr].factors[1] = primes_under_20[i];
        nr++;
    }

    return cases;
}

#endif /* not defined NDEBUG */

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

