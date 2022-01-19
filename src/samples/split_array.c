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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <math.h>   /* for fabs() */

/* genenrate a random array */
static void generate_random_array(long int *the_array, size_t the_size)
{
    srandom(time(NULL));

    for (size_t i = 0; i < the_size; i++) {
        // limit the values between [0, the_size) to avoid too big values
        the_array[i] = random() % the_size;
    }
}

/* print the long int array in C syntax to stderr */
static void print_array(const char *name, long int *the_array, size_t the_size)
{
    double d = 0;

    fprintf(stderr, "static long int %s[] = {\n", name);
    for (size_t i = 0; i < the_size; i++) {
        fprintf(stderr, "    %ld,\n", the_array[i]);
        d += the_array[i];
    }
    fprintf(stderr, "};\n");
    fprintf(stderr, "// Totally %lu elements; Sum: %f.\n\n", the_size, d);
}

/* the compare callback for qsort */
static int my_compare(const void * p1, const void * p2)
{
    long int a, b;

    a = *(const long int *)p1;
    b = *(const long int *)p2;
    return a - b;
}

/* split an long int array into two half arrays */
static size_t split_array(long int *the_array, size_t the_size,
        long int *first_half_array, long int *second_half_array)
{
    // sort the array first
    qsort(the_array, the_size, sizeof(*the_array), my_compare);

    size_t nr_split = 0;
    size_t n = 0;
    size_t len_1st = 0, len_2nd = 0;

    // split the arrary then
    while (nr_split < the_size) {
        first_half_array[len_1st++] = the_array[n * 2];
        first_half_array[len_1st++] = the_array[the_size - n * 2 - 1];
        nr_split += 2;

        if (nr_split < the_size) {
            second_half_array[len_2nd++] = the_array[n * 2 + 1];
            second_half_array[len_2nd++] = the_array[the_size - n * 2 - 2];
            nr_split += 2;
        }

        n++;
    }

    // make sure the two half arrays have the same length
    if (len_1st > len_2nd) {
        assert(len_1st == len_2nd + 2);

        len_1st--;
        second_half_array[len_2nd++] = first_half_array[len_1st];
    }
    else if (len_1st < len_2nd) {
        assert(len_2nd == len_1st + 2);

        len_2nd--;
        first_half_array[len_1st++] = second_half_array[len_2nd];
    }

    // make sure two half arrays have the same length
    assert(len_1st == len_2nd);
    return len_1st;
}

static double calc_diff_f(long int *first_half_array,
        long int *second_half_array, size_t half_size)
{
    double d1 = 0.0, d2 = 0.0;
    for (size_t i = 0; i < half_size; i++) {
        d1 += (double)first_half_array[i];
        d2 += (double)second_half_array[i];
    }

    return d1 - d2;
}

static long int calc_diff_l(long int *first_half_array,
        long int *second_half_array, size_t half_size)
{
    long int diff = 0;

    for (size_t i = 0; i < half_size; i++) {
        // accumulate the difference one by one to avoid overflow
        diff += (first_half_array[i] - second_half_array[i]);
    }

    return diff;
}

/* try a test case */
static void try_test_case(long *the_array, size_t the_size, long desired_diff)
{
    assert(the_size > 1);

    long int first_half_array[the_size / 2 + 2];
    long int second_half_array[the_size / 2 + 2];

    size_t half_size =
    split_array(the_array, the_size, first_half_array, second_half_array);

    double diff_f = calc_diff_f(first_half_array, second_half_array, half_size);
    assert(fabs(diff_f) == fabs((double)desired_diff));

    long diff_l = calc_diff_l(first_half_array, second_half_array, half_size);
    assert(abs(diff_l) == abs(desired_diff));
}

int main(void)
{
    long int *the_array = NULL;
    long int *first_half_array = NULL, *second_half_array = NULL;
    size_t the_size;
    int ret;

#ifndef NDEBUG
    {
#define NR_NUMBERS  100
        long int test_arrays[][NR_NUMBERS] = {
            /*
               use the first member for the number of the values
               use the second member for the diff */
            {2, 1, 1, 2},
            {4, 0, 1, 2, 3, 4},
            {8, 0, 200, 300, 800, 700, 500, 600, 400, 100},
            {8, 0, 2, 3, 8, 7, 5, 6, 4, 1},
        };

        for (size_t i = 0;
                i < sizeof(test_arrays)/sizeof(test_arrays[0]); i++) {
            try_test_case(test_arrays[i] + 2,
                    test_arrays[i][0], test_arrays[i][1]);
        }
    }
#endif /* not defined NDEBUG */

    do {
        unsigned int u;

        errno = 0;
        printf("Please input the length of a long int array (2～%u)\n", -1);
        ret = scanf("%u", &u);

        if (ret != 1) {
            int ch;
            do {
                ch = getchar();
            } while (ch != '\n');
        }

        the_size = u & (~0x01);
    } while (errno == ERANGE || ret != 1 || the_size < 2);

    printf("Allocating arrays (%lu * 2)...\n", the_size);
    the_array = calloc(the_size, sizeof(*the_array));
    if (the_array == NULL)
        goto failed;

    first_half_array = calloc(the_size / 2 + 2, sizeof(*first_half_array));
    if (first_half_array == NULL)
        goto failed;

    second_half_array = calloc(the_size / 2 + 2, sizeof(*second_half_array));
    if (second_half_array == NULL)
        goto failed;

    printf("Generating a random long int array with %lu slots...\n", the_size);
    generate_random_array(the_array, the_size);

    printf("Splitting the array...\n");
    size_t half_size =
        split_array(the_array, the_size, first_half_array, second_half_array);

    double diff_f = calc_diff_f(first_half_array, second_half_array, half_size);
    printf("The difference in double of two half arrays: %f\n", diff_f);

    long diff_l = calc_diff_l(first_half_array, second_half_array, half_size);
    printf("The difference in long of two half arrays: %ld\n", diff_l);

    print_array("original_array", the_array, the_size);
    print_array("first_half_array", first_half_array, half_size);
    print_array("second_half_array", second_half_array, half_size);

    free(the_array);
    free(first_half_array);
    free(second_half_array);
    return EXIT_SUCCESS;

failed:
    printf("Failed to allocate enough space for arrays (%lu)\n", the_size);
    if (the_array)
        free(the_array);
    if (first_half_array)
        free(first_half_array);
    if (second_half_array)
        free(second_half_array);

    return EXIT_FAILURE;
}

