#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

void generate_random_array(long int *the_array, size_t the_size)
{
    srandom(time(NULL));

    for (size_t i = 0; i < the_size; i++) {
        the_array[i] = random();
    }
}

static int my_compare(const void * p1, const void * p2)
{
    long int a, b;

    a = *(const long int *)p1;
    b = *(const long int *)p2;
    return a - b;
}

int main(void)
{
    long int *the_array = NULL;
    long int *first_half_array = NULL, *second_half_array = NULL;
    size_t the_size;
    int ret;

    do {
        unsigned int u;

        errno = 0;
        printf("Please the length of the long int array (0～%u)\n", -1);
        ret = scanf("%u", &u);

        if (ret != 1) {
            int ch;
            do {
                ch = getchar();
            } while (ch != '\n');
        }

        the_size = u & (~0x01);
    } while (errno == ERANGE || ret != 1);

    the_array = calloc(the_size, sizeof(*the_array));
    if (the_array == NULL)
        goto failed;

    first_half_array = calloc(the_size / 2 + 2, sizeof(*first_half_array));
    if (first_half_array == NULL)
        goto failed;

    second_half_array = calloc(the_size / 2 + 2, sizeof(*second_half_array));
    if (second_half_array == NULL)
        goto failed;

    printf("Generating a random long int array with %lu slot...\n", the_size);
    generate_random_array(the_array, the_size);

    printf("Sorting the array...\n");
    qsort(the_array, the_size, sizeof(*the_array), my_compare);

    size_t left = the_size;
    size_t n = 0;
    size_t len_1st = 0, len_2nd = 0;

    while (left > 0) {
        if (n % 2 == 0) {
            first_half_array[len_1st++] = the_array[n * 2];
            first_half_array[len_1st++] = the_array[n * 2 + 1];
        }
        else {
            second_half_array[len_2nd++] = the_array[n * 2];
            second_half_array[len_2nd++] = the_array[n * 2 + 1];
        }

        len_1st++;
        left -= 2;
    }

    if (len_1st > len_2nd) {
        assert(len_1st == len_2nd + 2);

        len_1st--;
        second_half_array[len_2nd++] = first_half_array[len_1st - 1];
    }
    else if (len_1st < len_2nd) {
        assert(len_2nd == len_1st + 2);

        len_2nd--;
        first_half_array[len_1st++] = second_half_array[len_2nd - 1];
    }

    assert(len_1st == len_2nd);

    double d1 = 0.0, d2 = 0.0;
    for (size_t i = 0; i < len_1st; i++) {
        d1 += (double)first_half_array[i];
        d2 += (double)second_half_array[i];
    }

    printf("The difference of two half arrays: %f\n", d1 - d2);

    free(the_array);
    free(first_half_array);
    free(second_half_array);
    return EXIT_SUCCESS;

failed:
    if (the_array)
        free(the_array);
    if (first_half_array)
        free(first_half_array);
    if (second_half_array)
        free(second_half_array);

    return EXIT_FAILURE;
}
