## 第十一讲：单元测试

- 时间：2022 年 1 月 27 日（周四）17：30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 单元测试的基本概念和重要性
1. 单元测试可以无处不在
1. 单元测试框架
1. 实例研究

		
## 单元测试的基本概念

1. 软件质量的第一责任人是程序员，而非测试员
1. 单元测试用来保证软件的基本质量，通过测试单个接口或模块（一组接口）的基本功能来确保：
   - 正确性：确保程序按照预期工作，尤其满足特定的边界条件
   - 性能：确保空间复杂度和性能复杂度在预期范围内
   - 无重大缺陷：确保无内存泄露、缓冲区溢出等重要缺陷

	
### 术语

1. 测试（test）：用于验证接口或者模块是否按照预期工作的活动
1. 测试用例（test case）：用于测试特定功能的实例，通常包含一组输入数据以及对应的预期结果
1. 测试套件（test suite）：对多个测试用例的分组
1. 测试方法（test method）：如何测试某个接口或模块，也就是如何设计和利用测试用例
1. 测试框架（test framework）：专用于自动化单元测试的软件框架，如 GLib Testing、GoogleTest 等

		
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

/* the compare callback for qsort */
static int my_compare(const void * p1, const void * p2)
{
    long int a, b;

    a = *(const long int *)p1;
    b = *(const long int *)p2;
    return a - b;
}

/* split a long int array into two half arrays */
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

#ifndef NDEBUG
/* 通过测试用例检查 split_array 函数的正确性 */
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
#endif /* not defined NDEBUG */

...

#ifndef NDEBUG
    {
#define NR_NUMBERS  100
        long int test_arrays[][NR_NUMBERS] = {
            /* 这个数组中的每个成员是一个测试用例。
               每个测试用例使用一个数组表示（成员至多 NR_NUMBERS 个）；
               其中第一个成员表示要分割的数组成员数量，
               第二个成员表示预期的分割后的两个数组成员之和的差，
               其后是要分割的数组成员。 */
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
```

		
## 单元测试中什么最重要？

1. 测试用例：要包含哪些用例才能覆盖尽可能多的边界条件？
1. 测试方法：如何设计测试方法，使之可以自动生成测试用例？

		
## 自动生成测试用例

分解自然数质数因子的函数

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
```

	

我们利用已知的质数反向计算结果，然后来测试分解函数的正确性

```c
#ifdef ENABLE_UNIT_TEST

#define SZ_TABLE(array)     (sizeof(array)/sizeof(array[0]))

// 所有小于 20 的质数
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

    /* 每个小于 20 的质数作为一个测试用例 */
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {

        EXPAND_SPACE;

        cases[nr].natural = primes_under_20[i];
        cases[nr].nr_factors = 1;
        cases[nr].factors[0] = primes_under_20[i];

        nr++;
    }

    /* 所有小于 20 的质数的乘积作为一个测试用例 */
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

    /* 每个小于 20 的质数的平方作为一个测试用例 */
    for (size_t i = 0; i < SZ_TABLE(primes_under_20); i++) {

        EXPAND_SPACE;

        cases[nr].natural = primes_under_20[i] * primes_under_20[i];
        cases[nr].nr_factors = 1;
        cases[nr].factors[0] = primes_under_20[i];
        nr++;
    }

    /* 随机选择三个质数，用这些质数（每个质数至少出现一次）的乘积作为测试用例 */
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

#endif /* ENABLE_UNIT_TEST */

...


#ifdef ENABLE_UNIT_TEST
    {
        size_t nr_cases;
        struct prime_factors *cases;

        printf("Run unit test...\n");

        cases = generate_test_cases(&nr_cases);
        for (size_t i = 0; i < nr_cases; i++) {
            unsigned int nr_factors;
            unsigned int *factors;

            printf("Calculating the prime factor(s) of %u...\n", cases[i].natural);
            factors = prime_factors(cases[i].natural, &nr_factors);
            if (nr_factors != cases[i].nr_factors) {
                printf("Incorrect number of prime factors: %u (desired: %u)\n",
                        nr_factors, cases[i].nr_factors);
                exit(EXIT_FAILURE);
            }
            else if (memcmp (factors, cases[i].factors,
                        sizeof(unsigned int) * nr_factors)) {
                printf("Incorrect prime factors: ");
                for (size_t j = 0; j < nr_factors; j++) {
                    printf("%u, ", factors[j]);
                }
                printf("\n");

                printf("Desired prime factors: ");
                for (size_t j = 0; j < cases[i].nr_factors; j++) {
                    printf("%u, ", cases[i].factors[j]);
                }
                printf("\n");
                exit(EXIT_FAILURE);
            }
        }

        free(cases);

        printf("All test cases passed!\n\n");
    }
#endif /* ENABLE_UNIT_TEST */
```

	
运行效果

```
Run unit test...
Calculating the prime factor(s) of 2...
Calculating the prime factor(s) of 3...
Calculating the prime factor(s) of 5...
Calculating the prime factor(s) of 7...
Calculating the prime factor(s) of 11...
Calculating the prime factor(s) of 13...
Calculating the prime factor(s) of 17...
Calculating the prime factor(s) of 19...
Calculating the prime factor(s) of 9699690...
Calculating the prime factor(s) of 4...
Calculating the prime factor(s) of 9...
Calculating the prime factor(s) of 25...
Calculating the prime factor(s) of 49...
Calculating the prime factor(s) of 121...
Calculating the prime factor(s) of 169...
Calculating the prime factor(s) of 289...
Calculating the prime factor(s) of 361...
Calculating the prime factor(s) of 2166...
Calculating the prime factor(s) of 950...
Calculating the prime factor(s) of 35378...
Calculating the prime factor(s) of 836...
Calculating the prime factor(s) of 178334...
Calculating the prime factor(s) of 646...
Calculating the prime factor(s) of 12996...
Calculating the prime factor(s) of 190...
Calculating the prime factor(s) of 1862...
Calculating the prime factor(s) of 101156...
Calculating the prime factor(s) of 6422...
Calculating the prime factor(s) of 646...
Calculating the prime factor(s) of 2166...
Calculating the prime factor(s) of 190...
Calculating the prime factor(s) of 532...
Calculating the prime factor(s) of 150898...
Calculating the prime factor(s) of 494...
Calculating the prime factor(s) of 21964...
Calculating the prime factor(s) of 342...
Calculating the prime factor(s) of 9500...
Calculating the prime factor(s) of 20216...
Calculating the prime factor(s) of 15884...
Calculating the prime factor(s) of 83486...
Calculating the prime factor(s) of 67396534...
Calculating the prime factor(s) of 684...
Calculating the prime factor(s) of 137180...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 15884...
Calculating the prime factor(s) of 37544...
Calculating the prime factor(s) of 3964502...
Calculating the prime factor(s) of 12996...
Calculating the prime factor(s) of 380...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 4598...
Calculating the prime factor(s) of 122018...
Calculating the prime factor(s) of 21964...
Calculating the prime factor(s) of 456...
Calculating the prime factor(s) of 1900...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 418...
Calculating the prime factor(s) of 988...
Calculating the prime factor(s) of 646...
Calculating the prime factor(s) of 342...
Calculating the prime factor(s) of 950...
Calculating the prime factor(s) of 638666...
Calculating the prime factor(s) of 960982...
Calculating the prime factor(s) of 12844...
Calculating the prime factor(s) of 10982...
Calculating the prime factor(s) of 4332...
Calculating the prime factor(s) of 1900...
Calculating the prime factor(s) of 7448...
Calculating the prime factor(s) of 836...
Calculating the prime factor(s) of 9386...
Calculating the prime factor(s) of 417316...
Calculating the prime factor(s) of 2166...
Calculating the prime factor(s) of 380...
Calculating the prime factor(s) of 672182...
Calculating the prime factor(s) of 418...
Calculating the prime factor(s) of 1976...
Calculating the prime factor(s) of 233206...
Calculating the prime factor(s) of 684...
Calculating the prime factor(s) of 90250...
Calculating the prime factor(s) of 1862...
Calculating the prime factor(s) of 4598...
Calculating the prime factor(s) of 9386...
Calculating the prime factor(s) of 186694...
Calculating the prime factor(s) of 684...
Calculating the prime factor(s) of 1900...
Calculating the prime factor(s) of 35378...
Calculating the prime factor(s) of 1921964...
Calculating the prime factor(s) of 2318342...
Calculating the prime factor(s) of 1292...
Calculating the prime factor(s) of 456...
Calculating the prime factor(s) of 380...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 418...
Calculating the prime factor(s) of 494...
Calculating the prime factor(s) of 10982...
Calculating the prime factor(s) of 82308...
Calculating the prime factor(s) of 3610...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 174724...
Calculating the prime factor(s) of 6422...
Calculating the prime factor(s) of 12274...
Calculating the prime factor(s) of 1368...
Calculating the prime factor(s) of 7220...
Calculating the prime factor(s) of 266...
Calculating the prime factor(s) of 4598...
Calculating the prime factor(s) of 37544...
Calculating the prime factor(s) of 1292...
Calculating the prime factor(s) of 684...
Calculating the prime factor(s) of 190...
Calculating the prime factor(s) of 40432...
Calculating the prime factor(s) of 4598...
Calculating the prime factor(s) of 494...
Calculating the prime factor(s) of 417316...
Calculating the prime factor(s) of 6498...
Calculating the prime factor(s) of 190...
Calculating the prime factor(s) of 1862...
Calculating the prime factor(s) of 7942...
All test cases passed!
```

		
## 单元测试框架

1. 单元测试框架只是一个辅助性工具
1. 不能解决如何测试的问题
1. 也不能自动生成测试用例

	
### 单元测试框架通常包含什么？

1. 定义一个测试的便利方法
1. 测试用例组织成测试套件的便利方法
1. 各种方便对比运行结果和预期结果的断言宏
1. 测试报告（是否通过、运行时长等信息）

	
### GLib Testing

1. 使用夹具（Fixture）的概念，用于封装测试对象以及测试数据。每一次测试前后，分别执行夹具的设置（set up）和拆卸（tear down）操作。
1. 使用类似路径的形式定义测试套件和测试用例：`/myobject/test1`。

	
### GLib Testing 提供的断言宏

1. `g_assert_true()`
1. `g_assert_cmpint()`, `g_assert_cmpuint()`
1. `g_assert_cmpfloat()`, `g_assert_cmpfloat_with_epsilon()`
1. `g_assert_cmphex()`, `g_assert_cmpstr()`, `g_assert_cmpmem()`, and `g_assert_cmpvariant()`

	
### GLib Testing 示例（来源：<https://docs.gtk.org/glib/testing.html>）

```
#include <glib.h>
#include <locale.h>

typedef struct {
  MyObject *obj;
  OtherObject *helper;
} MyObjectFixture;

static void
my_object_fixture_set_up (MyObjectFixture *fixture,
                          gconstpointer user_data)
{
  fixture->obj = my_object_new ();
  my_object_set_prop1 (fixture->obj, "some-value");
  my_object_do_some_complex_setup (fixture->obj, user_data);

  fixture->helper = other_object_new ();
}

static void
my_object_fixture_tear_down (MyObjectFixture *fixture,
                             gconstpointer user_data)
{
  g_clear_object (&fixture->helper);
  g_clear_object (&fixture->obj);
}

static void
test_my_object_test1 (MyObjectFixture *fixture,
                      gconstpointer user_data)
{
  g_assert_cmpstr (my_object_get_property (fixture->obj), ==, "initial-value");
}

static void
test_my_object_test2 (MyObjectFixture *fixture,
                      gconstpointer user_data)
{
  my_object_do_some_work_using_helper (fixture->obj, fixture->helper);
  g_assert_cmpstr (my_object_get_property (fixture->obj), ==, "updated-value");
}

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);

  // Define the tests.
  g_test_add ("/my-object/test1", MyObjectFixture, "some-user-data",
              my_object_fixture_set_up, test_my_object_test1,
              my_object_fixture_tear_down);
  g_test_add ("/my-object/test2", MyObjectFixture, "some-user-data",
              my_object_fixture_set_up, test_my_object_test2,
              my_object_fixture_tear_down);

  return g_test_run ();
}
```

	
### GoogleTest

1. Google 的 C++ 测试（testing）和模拟（mocking）框架
1. 也可以用来测试 C 接口和模块
1. 比 GLib Testing 更完善更强大
1. 文档：<http://google.github.io/googletest/>

		
## 实例研究

	
### HVML MATH 对象四则运算表达式的求值

```php
// 原型
$MATH.eval(<string: a four arithmetic expressions>[, <object: parameter map>]) : number

// 示例1：求解 (500 + 10) * (700 + 30)
$MATH.eval("(500 + 10) * (700 + 30)")

// 示例2：求圆的周长
$MATH.eval("2 * pi * r", { pi: $MATH.pi, r: $r })

// 示例3：求圆的面积
$MATH.eval("pi * r * r", { pi: $MATH.pi, r: $MATH.sqrt(2) })

// 原型：eval_l 的 long double 版本
$MATH.eval_l(<string: a four arithmetic expressions>[, <object: parameter map>]) : longdouble
```

	
### 测试方法

1. 使用 GNU 的 `bc` 工具（任意精度计算器）对同一表达式进行求值，将其执行结果作为预期结果。
1. 用以上的预期结果和 `MATH` 对象 `eval` 方法的结果进行对比。
1. 手工整理尽可能覆盖各种边界条件的测试用例，包括正面（positive）用例和负面（negative）用例。

	
### 测试代码

```c
#include <math.h>
#include <sstream>
#include <gtest/gtest.h>


static inline bool
long_double_eq(long double l, long double r)
{
    long double lp = fabsl(l);
    long double rp = fabsl(r);
    long double maxp = lp > rp ? lp : rp;
    return fabsl(lp - rp) <= maxp * FLT_EPSILON;
}

TEST(dvobjs, dvobjs_math_bc)
{
    int r = 0;
    DIR *d = NULL;
    struct dirent *dir = NULL;
    char path[1024] = {0};

    purc_instance_extra_info info = {};
    r = purc_init("cn.fmsoft.hybridos.test",
        "dvobjs_math_bc", &info);
    EXPECT_EQ(r, PURC_ERROR_OK);
    if (r)
        return;

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "eval", false);
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method func = NULL;
    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    char math_path[PATH_MAX+1];
    env = "DVOBJS_TEST_PATH";
    test_getpath_from_env_or_rel(math_path, sizeof(math_path),
        env, "test_files");
    std::cerr << "env: " << env << "=" << math_path << std::endl;

    strcpy (path, math_path);
    strcat (path, "/math_bc");

    d = opendir(path);
    EXPECT_NE(d, nullptr) << "Failed to open dir @["
            << path << "]: [" << errno << "]" << strerror(errno)
            << std::endl;

    if (d) {
        if (chdir(path) != 0) {
            purc_variant_unref(math);
            return;
        }
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type & DT_REG) {
                std::stringstream ss;
                ss << "bc file:[" << dir->d_name << "][";
                long double l, r;
                _process_file(func, dir->d_name, &l, ss);
                ss << "]-[";
                _eval_bc(dir->d_name, &r, ss);
                ss << "]";
                ss << "==?==[" << fabsl(l-r) << "]";
                std::cout << ss.str() << std::endl;
                EXPECT_TRUE(long_double_eq(l, r))
                    << "Failed to parse bc file: ["
                    << dir->d_name << "]"
                    << std::endl;
            }
        }
        closedir(d);
    }

    if (math)
        purc_variant_unload_dvobj (math);

    purc_cleanup ();
}
```

	
### 正面测试用例

```bc
# case 0
((3.4 - 5.4) / (2.3 + 7.8) + (6.2 + 3.4) * (2.3 / 2.0)) + (4.3 * 3.4)

# case 1
x = 3
y = 9
x * y
```

	
### 负面测试用例

```bc
# case 0
5 - (6 * 7

# case 1
x = 3
y = 0
x / y
```

	
### 测试结果

```
./Source/test/dvobjs/test_math
Running main() from /srv/devel/hvml/purc/Source/ThirdParty/gtest/googletest/src/gtest_main.cc
[==========] Running 7 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 7 tests from dvobjs
[ RUN      ] dvobjs.dvobjs_math_pi_e
env: DVOBJS_SO_PATH=/srv/devel/hvml/purc/Source/test/dvobjs/../../../build/Source/ExtDVObjs
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:159: Failure
Expected equality of these values:
  sz_total_mem_after
    Which is: 2240
  sz_total_mem_before + (nr_reserved_after - nr_reserved_before) * sizeof(purc_variant)
    Which is: 1744
[  FAILED  ] dvobjs.dvobjs_math_pi_e (0 ms)
[ RUN      ] dvobjs.dvobjs_math_const
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:312: Failure
Expected equality of these values:
  ret
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_const (0 ms)
[ RUN      ] dvobjs.dvobjs_math_func
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:667: Failure
Expected equality of these values:
  ret
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_func (0 ms)
[ RUN      ] dvobjs.dvobjs_math_eval
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:761: Failure
Expected equality of these values:
  ret
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_eval (0 ms)
[ RUN      ] dvobjs.dvobjs_math_assignment
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:908: Failure
Expected equality of these values:
  ret
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_assignment (0 ms)
[ RUN      ] dvobjs.dvobjs_math_samples
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:995: Failure
Expected equality of these values:
  ret
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_samples (0 ms)
[ RUN      ] dvobjs.dvobjs_math_bc
/srv/devel/hvml/purc/Source/test/dvobjs/test_math.cpp:1185: Failure
Expected equality of these values:
  r
    Which is: 5
  PURC_ERROR_OK
    Which is: 0
[  FAILED  ] dvobjs.dvobjs_math_bc (0 ms)
[----------] 7 tests from dvobjs (1 ms total)

[----------] Global test environment tear-down
[==========] 7 tests from 1 test suite ran. (1 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 7 tests, listed below:
[  FAILED  ] dvobjs.dvobjs_math_pi_e
[  FAILED  ] dvobjs.dvobjs_math_const
[  FAILED  ] dvobjs.dvobjs_math_func
[  FAILED  ] dvobjs.dvobjs_math_eval
[  FAILED  ] dvobjs.dvobjs_math_assignment
[  FAILED  ] dvobjs.dvobjs_math_samples
[  FAILED  ] dvobjs.dvobjs_math_bc

 7 FAILED TESTS
```

		
## 下一讲预告

- 主题：高效调试
- 地点：微信视频号“考鼎录”直播间
- 时间：2022 年 1 月 29 日（周六）17：30

		
## Q & A

