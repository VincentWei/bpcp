## 第十一讲：单元测试

- 时间：2022 年 1 月 27 日（周四）17：30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 单元测试的基本概念和重要性
1. 单元测试可以无处不在
1. 单元测试中什么最重要？
1. 编码生成测试用例
1. 单元测试框架
1. 实例研究

		
## 单元测试的基本概念

1. 软件质量的第一责任人是程序员，而非测试员
1. 单元测试用来保证软件的基本质量，通过测试单个接口或模块（一组接口）的基本功能来确保：
   - 正确性：确保程序按照预期工作，尤其满足特定的边界条件
   - 性能：确保空间复杂度和性能复杂度在预期范围内
   - 无重大缺陷：确保无内存泄露、缓冲区溢出等重要缺陷

	
### 术语

1. 测试（test）：用于验证接口或者模块是否按照预期工作的活动/行为/代码
1. 测试用例（test case）：用于测试特定功能的实例；一个测试用例通常包含一组输入数据以及对应的预期结果
1. 测试套件（test suite）：对多个测试用例的分组
1. 测试方法（test method）：如何测试某个接口或模块，也就是如何设计和使用测试用例
1. 测试框架（testing framework）：专用于自动化单元测试的软件框架，如 GLib Testing、GoogleTest 等

		
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

1. 测试用例：如何用尽可能少的测试用例覆盖尽可能多的边界条件？
1. 测试方法：如何设计测试方法，使之
   - 可以自动生成测试用例？或者，
   - 可以自动生成预期结果？或者，
   - 测试用例可由其他人无需编码即可维护？

		
## 编码生成测试用例

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

	
### GLib Testing 示例

来源：<https://docs.gtk.org/glib/testing.html>

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
[ RUN      ] dvobjs.dvobjs_math_bc
env: DVOBJS_SO_PATH=/srv/devel/hvml/purc/Source/test/dvobjs/../../../build/Source/ExtDVObjs
env: DVOBJS_TEST_PATH=/srv/devel/hvml/purc/Source/test/dvobjs/test_files
bc file:[21.bc][9485.74]-[9485.744]==?==[5.96856e-13]
bc file:[77.bc][-1.59609]-[-1.59609138180067404910]==?==[3.79362e-16]
bc file:[75.bc][-16.9092]-[-16.90915635545556805400]==?==[3.33067e-16]
bc file:[32.bc][61.7995]-[61.79946132739632319347]==?==[7.18176e-16]
bc file:[37.bc][266240]-[266239.91428571428571428518]==?==[1.33014e-11]
bc file:[91.bc][0.815385]-[.81538461538461538462]==?==[2.3229e-16]
bc file:[6.bc][1.23457e+09]-[1234567890.1234567890]==?==[7.24103e-08]
bc file:[38.bc][1007.71]-[1007.71411764705882352941]==?==[8.10463e-14]
bc file:[67.bc][5897.73]-[5897.73000000000000000000]==?==[1.38245e-12]
bc file:[22.bc][122.114]-[122.11412556627677817306]==?==[3.35218e-14]
bc file:[18.bc][-20604]-[-20603.96485623003194887646]==?==[2.20268e-13]
bc file:[43.bc][232.5]-[232.49978448275862068965]==?==[7.04992e-15]
bc file:[35.bc][-1649.45]-[-1649.45000000000000000000]==?==[4.55191e-14]
bc file:[30.bc][-127758]-[-127757.98400008000000000000]==?==[3.16902e-12]
bc file:[42.bc][516]-[516.00000000000000000000]==?==[0]
bc file:[20.bc][106747]-[106747.13037037037037037147]==?==[3.9222e-12]
bc file:[5.bc][7.52]-[7.52000000000000000000]==?==[4.6187e-16]
bc file:[36.bc][1534.11]-[1534.10656084656084656085]==?==[4.996e-14]
bc file:[87.bc][738.645]-[738.64516129032258064517]==?==[1.83187e-14]
bc file:[69.bc][36.2235]-[36.22347629796839729119]==?==[3.46459e-14]
bc file:[48.bc][-291002]-[-291002]==?==[0]
bc file:[81.bc][7.24198]-[7.24197825324447562258]==?==[1.60462e-17]
bc file:[27.bc][790.947]-[790.94654731457800511509]==?==[7.82707e-14]
bc file:[49.bc][-0.00199302]-[-.00199301907271145227]==?==[5.71747e-21]
bc file:[100.bc][595.916]-[595.91625338702611337714]==?==[1.31672e-13]
bc file:[12.bc][659.58]-[659.57976190476190476191]==?==[2.38143e-14]
bc file:[53.bc][411.009]-[411.00885469135802469136]==?==[1.43774e-14]
bc file:[44.bc][-1101]-[-1101.00000000000000000000]==?==[0]
bc file:[15.bc][876.638]-[876.63823529411764705882]==?==[1.20459e-14]
bc file:[52.bc][-2746.61]-[-2746.61481481481481481484]==?==[3.06644e-13]
bc file:[93.bc][32.7663]-[32.76633304082717628899]==?==[8.38565e-15]
bc file:[39.bc][786.04]-[786.04000000000000000000]==?==[3.63598e-14]
bc file:[99.bc][-1188.15]-[-1188.14851741387604218157]==?==[1.6287e-13]
bc file:[98.bc][173805]-[173804.64558048541811245469]==?==[1.59304e-11]
bc file:[83.bc][-37.2]-[-37.20]==?==[2.84148e-15]
bc file:[65.bc][-35210.6]-[-35210.56]==?==[2.32703e-12]
bc file:[31.bc][990.906]-[990.90612903225806451612]==?==[2.83107e-14]
bc file:[61.bc][276496]-[276496.22981029810298102982]==?==[3.1747e-11]
bc file:[25.bc][547.32]-[547.32]==?==[6.36713e-14]
bc file:[41.bc][-229.827]-[-229.82652274479568234387]==?==[2.78527e-14]
bc file:[2.bc][-11]-[-11]==?==[0]
bc file:[59.bc][223.724]-[223.72375009375234380859]==?==[1.44607e-14]
bc file:[23.bc][-164.752]-[-164.75213675213675213664]==?==[3.11001e-14]
bc file:[17.bc][363.76]-[363.76]==?==[1.61426e-13]
bc file:[57.bc][-798878]-[-798877.84000000000000000000]==?==[3.25713e-11]
bc file:[88.bc][19.351]-[19.35102359056377449021]==?==[9.66241e-16]
bc file:[95.bc][-803.714]-[-803.71385718595397053428]==?==[8.62643e-14]
bc file:[68.bc][0.748604]-[.74860431779382198081]==?==[9.1073e-18]
bc file:[84.bc][-117.444]-[-117.44398]==?==[1.04777e-14]
bc file:[82.bc][-551.23]-[-551.23000000000000000000]==?==[1.82077e-14]
bc file:[85.bc][44.0549]-[44.05488301031392705591]==?==[2.44596e-15]
bc file:[92.bc][-1473.81]-[-1473.81011655011655011396]==?==[1.5965e-13]
bc file:[7.bc][273]-[272.99999999999999999727]==?==[1.93268e-12]
Runtime error (func=(main), adr=17): Divide by zero
bc file:[11.bc][inf]-[]==?==[inf]
bc file:[50.bc][3160.09]-[3160.08814589665653495440]==?==[9.25926e-14]
bc file:[47.bc][154828]-[154828.12738095238095237499]==?==[3.60956e-12]
bc file:[97.bc][24.7064]-[24.70635271583738549013]==?==[1.079e-15]
bc file:[64.bc][-120.876]-[-120.87617805922890668653]==?==[2.94278e-14]
bc file:[4.bc][32.34]-[32.34]==?==[3.69496e-15]
bc file:[96.bc][684.205]-[684.20485676953181538619]==?==[1.43774e-14]
bc file:[13.bc][2.1644]-[2.16439690638909249915]==?==[1.57209e-16]
bc file:[66.bc][24891.9]-[24891.89803921568627450981]==?==[1.72662e-12]
bc file:[19.bc][-435.4]-[-435.40000000000000000000]==?==[3.41116e-14]
bc file:[94.bc][19.9472]-[19.94715532809092221695]==?==[2.61943e-16]
bc file:[10.bc][11.28]-[11.28000000000000000000]==?==[1.13711e-15]
bc file:[62.bc][273.983]-[273.98335315101070154346]==?==[2.10942e-15]
bc file:[54.bc][-118.842]-[-118.84225980295566502464]==?==[1.25178e-14]
bc file:[29.bc][-854464]-[-854463.900]==?==[9.31095e-11]
bc file:[63.bc][1864.92]-[1864.92230769230769230769]==?==[5.17364e-14]
bc file:[90.bc][-291.016]-[-291.01620000000000000000]==?==[3.06422e-14]
bc file:[16.bc][-387.5]-[-387.5]==?==[0]
bc file:[89.bc][474.473]-[474.47302904564315352697]==?==[5.41234e-15]
bc file:[45.bc][1.85478]-[1.85478489903424056189]==?==[3.5345e-17]
bc file:[14.bc][-271.589]-[-271.58879322126554598880]==?==[2.72837e-14]
bc file:[79.bc][26.8806]-[26.88056975013076892125]==?==[1.26288e-15]
bc file:[28.bc][24321]-[24321]==?==[0]
bc file:[80.bc][1.02625]-[1.02624704565244433387]==?==[5.10876e-16]
bc file:[34.bc][1545.6]-[1545.59616614415173209802]==?==[3.41949e-14]
bc file:[60.bc][-907.01]-[-907.01000000000000000000]==?==[1.04583e-13]
bc file:[86.bc][18.006]-[18.00601521180677983177]==?==[5.72459e-17]
bc file:[8.bc][3.88184]-[3.88183593750000000000]==?==[0]
bc file:[46.bc][1469.62]-[1469.61812865497076023391]==?==[1.00031e-13]
bc file:[3.bc][11]-[11.00000000000000000000]==?==[0]
bc file:[9.bc][25.462]-[25.46198019801980198020]==?==[3.29597e-17]
bc file:[24.bc][-87162.3]-[-87162.30]==?==[1.16387e-11]
bc file:[71.bc][-701.816]-[-701.81647058823529411765]==?==[5.32352e-14]
bc file:[55.bc][32.035]-[32.03501577287066246056]==?==[2.35922e-15]
bc file:[76.bc][10092]-[10092.00000000000000000000]==?==[0]
bc file:[33.bc][-1.13874]-[-1.13873684210526315789]==?==[3.32525e-16]
bc file:[72.bc][-1232.22]-[-1232.216]==?==[1.05471e-13]
bc file:[26.bc][954.866]-[954.86635205285512033979]==?==[5.67879e-14]
bc file:[51.bc][-89122.8]-[-89122.79076086956521739131]==?==[2.52953e-12]
bc file:[73.bc][9.51042]-[9.51042180154696343432]==?==[3.95517e-16]
bc file:[70.bc][879.46]-[879.46000000000000000000]==?==[3.63598e-14]
bc file:[40.bc][301.594]-[301.59402759495875685471]==?==[8.199e-14]
bc file:[56.bc][12.1233]-[12.12333333333333333334]==?==[1.51615e-15]
bc file:[74.bc][-62.9894]-[-62.98942878000000000000]==?==[2.76862e-15]
bc file:[1.bc][-9]-[-9]==?==[0]
bc file:[78.bc][971.002]-[971.00181038589804668889]==?==[3.54716e-14]
bc file:[58.bc][-57.3613]-[-57.36134283648383431463]==?==[2.25861e-14]
[       OK ] dvobjs.dvobjs_math_bc (137 ms)
```

		
## 下一讲预告

- 主题：高效调试
- 地点：微信视频号“考鼎录”直播间
- 时间：2022 年 1 月 29 日（周六）17：30

		
## Q & A

