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
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <locale.h>
#include <glib.h>

#define DEFSZ_FACTORS   4

unsigned int *prime_factors(unsigned int natural, unsigned int *nr_factors)
{
    unsigned int *factors = NULL;
    unsigned int sz = DEFSZ_FACTORS;

    assert(nr_factors);

    if (natural < 2) {
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
    if (nr >= sz) {             \
        sz += DEF_SIZE;         \
        cases = realloc(cases, sizeof(struct prime_factors) * sz);  \
        assert(cases != NULL);  \
    }                           \
} while(0)

static struct prime_factors *generate_test_cases(size_t *nr_cases)
{
    size_t nr = 0, sz = 0;
    struct prime_factors *cases = NULL;

    assert(nr_cases != NULL);

    srandom(time(NULL));

    /* the test cases for the prime numbers themselves */
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
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {
        cases[nr].natural *= primes_under_20[i];
    }
    cases[nr].nr_factors = SZ_TABLE(primes_under_20);
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {
        cases[nr].factors[i] = primes_under_20[i];
    }
    nr++;

    /* the test cases of the squres of the prime numbers under 20 */
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {

        EXPAND_SPACE;

        cases[nr].natural = primes_under_20[i] * primes_under_20[i];
        cases[nr].nr_factors = 1;
        cases[nr].factors[0] = primes_under_20[i];
        nr++;
    }

    /* Some random test cases */
    for (size_t i = 0; i < 100; i++) {
        EXPAND_SPACE;

        cases[nr].nr_factors = 3;
        cases[nr].factors[0] = primes_under_20[0];
        cases[nr].factors[1] = primes_under_20[
            1 + (i % (SZ_TABLE(primes_under_20) - 2))];
        cases[nr].factors[2] = primes_under_20[SZ_TABLE(primes_under_20) - 1];

        cases[nr].natural = 1;
        cases[nr].natural *= cases[nr].factors[0];
        cases[nr].natural *= cases[nr].factors[1];
        cases[nr].natural *= cases[nr].factors[2];
        for (size_t j = 0; j < random() % 5; j++) {
            cases[nr].natural *= cases[nr].factors[random() % 3];
        }

        nr++;
    }

    *nr_cases = nr;
    return cases;
}

/* 针对 prime_factors() 的夹具对象类型 */
typedef struct {
    struct prime_factors *cases;
    size_t nr_cases;
} PrimeFactorsFixture;

/* 夹具设置函数 */
static void
prime_factors_fixture_set_up(PrimeFactorsFixture *fixture,
        gconstpointer user_data)
{
    (void)user_data;
    fixture->cases = generate_test_cases(&fixture->nr_cases);
}

/* 夹具清理函数 */
static void
prime_factors_fixture_tear_down(PrimeFactorsFixture *fixture,
        gconstpointer user_data)
{
    (void)user_data;
    free(fixture->cases);
}

static void
prime_factors_test(PrimeFactorsFixture *fixture, gconstpointer user_data)
{
    size_t nr_cases = fixture->nr_cases;
    struct prime_factors *cases = fixture->cases;

    for (size_t i = 0; i < nr_cases; i++) {
        unsigned int nr_factors;
        unsigned int *factors;

        // 调用 prime_factors() 进行质因子分解
        factors = prime_factors(cases[i].natural, &nr_factors);

        // 对比结果
        g_assert_true(nr_factors == cases[i].nr_factors);
        g_assert_cmpmem(factors, sizeof(unsigned int) * nr_factors,
                cases[i].factors, sizeof(unsigned int) * nr_factors);
    }
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    g_test_init(&argc, &argv, NULL);

    g_test_add("/prime-factor/test", PrimeFactorsFixture, NULL,
            prime_factors_fixture_set_up, prime_factors_test,
            prime_factors_fixture_tear_down);

    return g_test_run ();
}
