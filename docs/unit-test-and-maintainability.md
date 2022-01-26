## 第十一讲：单元测试及可维护性

- 时间：2022 年 1 月 27 日（周四）17：30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 单元测试的基本概念和重要性
1. 单元测试可以无处不在
1. 单元测试框架
1. 实例研究

		
## 单元测试的基本概念

1. 软件质量的第一责任人是程序员
1. 单元测试用来保证软件的基本质量，通过测试单个接口或模块（一组接口）的基本功能来确保：
   - 正确性：确保程序按照预期工作，尤其满足给定的边界条件
   - 性能：确保空间复杂度和性能复杂度在预期范围内
   - 无重大缺陷：确保无内存泄露、缓冲区溢出等重要缺陷
1. 术语：
   - 测试用例（test case）：用于测试特定条件和功能的实例
   - 测试套件（test suite）：对对个测试用户的分组
   - 测试方法（test method）：如何测试某个接口或模块，也就是如何设计测试用例
   - 测试框架（test framework）：专用于自动化单元测试的软件框架，如 Glib Test、Google Test 等

		
## 单元测试的重要性

1. 近几年各种开源基础软件的安全性漏洞造成的破坏性越来越大
   - 2014 年 OpenSSL 漏洞
   - 2021 年 log4j 漏洞
1. 绝大多数开源软件缺乏基本的单元测试
   - 开发者太过自信，不重视测试的重要性
   - 急着开源然后被动等待用户提 `issue` 的“开源协作”模式容易产生低质量代码

		
## 单元测试可以无处不在

```c
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

    printf("sizeof dynamic array: %lu\n", sizeof(first_half_array));

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

```

		
## 单元测试中什么最重要？

1. 测试方法：如何设计测试方法，使之可以自动完成或者编码完成？
1. 测试用例：要包含哪些用例才能覆盖尽可能多的边界条件？

		
## 自动生成测试用例

```c
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
```

		
## 下一讲预告

- 主题：高效调试
- 地点：微信视频号“考鼎录”直播间
- 时间：2022 年 1 月 29 日（周六）17：30

		
## Q & A

