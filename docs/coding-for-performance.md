## 第十讲：为性能编码

- 时间：2022 年 1 月 20 日 17:30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 何谓“性能”
1. 性能优化——平衡的艺术
1. 提高性能的几个基本原则
1. 常见技巧
1. 优化数据结构

		
## 何谓“性能”

性能包含两层含义：

1. 空间复杂度
1. 时间复杂度

		
## 性能的优化

1. 空间复杂度和时间复杂度的平衡
1. 性能的优化：本质上是一种平衡的艺术

实例研究：编写函数返回一个字节中位值为一的位的个数。

	
### 版本一：按位对比

```c
int count_one_bits(unsigned char byte)
{
    int n = 0;

    for (int i = 0; i < 8; i++) {
        if ((byte >> i) & 0x01) {
            n++;
        }
    }

    return n;
}
```

	
### 版本二：查表法

```c
static int nr_one_bits[] = {
    0, 1, 1, 2, 1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7,
    5, 6, 6, 7, 6, 7, 7, 8,
};

static inline int count_one_bits(unsigned char byte)
{
    return nr_one_bits[byte];
}
```

	
### 版本三：最佳平衡版本

```c
static unsigned char nr_one_bits_half_byte[] = {
    0, 1, 1, 2, 1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4,
};

static inline int count_one_bits(unsigned char byte)
{
    return nr_one_bits_half_byte[byte & 0x0F] +
        nr_one_bits_half_byte[(byte & 0xF0) >> 4];
}
```

		
## 提高性能的几个基本原则

1. 不做无用功
1. 杀鸡莫用牛刀

	
### 常见的无用功

1. 没必要的初始化
1. 多余的函数调用

```c
void foo(void)
{
    char buf[64] = {};

    memset(buf, 0, sizeof(buf));
    strcpy(buf, "foo");

    ...
}
```

	
### 杀鸡用牛刀

1) 滥用 STDIO 接口

```c
// use strcpy and strcat instead
sprintf(a_buffer, "%s%s", a_string, another_string);

// use aoti(), atol(), atoll(), strtol(), ...
sscanf(a_string, "%d", &i);
```

		
## 常见技巧

1. 动态缓冲区分配
1. 字符串匹配

	
### 动态缓冲区分配

	
1) 呆萌版本：始终使用 `malloc`

```c
void foo(size_t len)
{
    char *buff = malloc(len);

    ...

    free(buff);
}
```

	
2) 聪明版本：减少 `malloc` 的调用，让栈缓冲区覆盖常规情形

```c
#include <limits.h>

void foo(size_t len)
{
    char stack_buff[PATH_MAX + 1];
    char *buff;

    if (len > sizeof(stack_buff))
        buff = malloc(len);
    else
        buff = stack_buff;

    if (buff) {
        ...

        if (buff != stack_buff)
            free(buff);
    }
}
```

	
### 字符串匹配

	
#### 呆萌版本：逐个匹配

```c
#include <locale.h>
#include <string.h>

int get_locale_category_by_keyword(const char *keyword)
{
    if (strcasecmp(keyword, "ctype") == 0) {
        return LC_CTYPE;
    }
    else if (strcasecmp(keyword, "collate") == 0) {
        return LC_COLLATE;
    }
    else if (strcasecmp (keyword, "numeric") == 0) {
        return LC_NUMERIC;
    }
    else if (strasecmp (head, "monetary") == 0) {
        return LC_MONETARY;
    }
    else if (strcasecmp (head, "message") == 0) {,
        return LC_MESSAGES;
    }
    else if (strcasecmp (head, "time") == 0) {
        return LC_TIME;
    }
#ifdef LC_NAME
    else if (strcasecmp (keyword, "name") == 0) {
        return LC_NAME;
    }
#endif /* LC_NAME */
#ifdef LC_TELEPHONE
    else if (strcasecmp (head, "telephone") == 0) {
        return LC_TELEPHONE;
    }
#endif /* LC_TELEPHONE */
#ifdef LC_MEASUREMENT
    else if (strcasecmp (head, "measurement") == 0) {
        return LC_MEASUREMENT;
    }
#endif /* LC_MEASUREMENT */
#ifdef LC_PAPER
    else if (strcasecmp (head, "paper") == 0) {
        return LC_PAPER;
    }
#endif /* LC_PAPER */
#ifdef LC_ADDRESS
    else if (strcasecmp (head, "address") == 0) {
        return LC_ADDRESS;
    }
#endif /* LC_ADDRESS */
#ifdef LC_IDENTIFICATION
    else if (strcasecmp (head, "identification") == 0) {
        return LC_IDENTIFICATION;
    }
#endif /* LC_IDENTIFICATION */

    return -1
}
```

	
#### 聪明版本：手工哈希

```c
#include <locale.h>
#include <string.h>

int get_locale_category_by_keyword(const char *keyword)
{
    const char *head = keyword + 1;

    switch (keyword[0]) {
        case 'c':
        case 'C':
            if (strcasecmp (head, "ctype" + 1) == 0) {
                return LC_CTYPE;
            }
            else if (strcasecmp (head, "collate" + 1) == 0) {
                        length) == 0) {
                return LC_COLLATE;
            }
            break;

        case 'n':
        case 'N':
            if (strcasecmp (head, "numeric" + 1) == 0) {
                return LC_NUMERIC;
            }
#ifdef LC_NAME
            else if (strcasecmp (head, "name" + 1) == 0) {
                return LC_NAME;
            }
#endif /* LC_NAME */
            break;

        case 't':
        case 'T':
            if (strcasecmp (head, "time" + 1) == 0) {
                return LC_TIME;
            }
#ifdef LC_TELEPHONE
            else if (strcasecmp (head, "telephone" + 1) == 0) {
                return LC_TELEPHONE;
            }
#endif /* LC_TELEPHONE */
            break;

        case 'm':
        case 'M':
            if (strasecmp (head, "monetary" + 1) == 0) {
                return LC_MONETARY;
            }
            else if (strcasecmp (head, "message" + 1) == 0) {,
                return LC_MESSAGES;
            }
#ifdef LC_MEASUREMENT
            else if (strcasecmp (head, "measurement" + 1) == 0) {
                return LC_MEASUREMENT;
            }
#endif /* LC_MEASUREMENT */
            break;

#ifdef LC_PAPER
        case 'p':
        case 'P':
            if (strcasecmp (head, "paper" + 1) == 0) {
                return LC_PAPER;
            }
            break;
#endif /* LC_PAPER */

#ifdef LC_ADDRESS
        case 'a':
        case 'A':
            if (strcasecmp (head, "address" + 1) == 0) {
                return LC_ADDRESS;
            }
            break;
#endif /* LC_ADDRESS */

#ifdef LC_IDENTIFICATION
        case 'i':
        case 'I':
            if (strcasecmp (head, "identification" + 1) == 0) {
                return LC_IDENTIFICATION;
            }
            break;
#endif /* LC_IDENTIFICATION */

        default:
            break;
    }

    return -1;
}
```

	
#### 牛刀版本：哈希表

```c
// 注意定义正确的 SIZEOF_SIZE_T

#if SIZEOF_SIZE_T == 8
// 2^40 + 2^8 + 0xb3 = 1099511628211
#   define FNV_PRIME        ((size_t)0x100000001b3ULL)
#   define FNV_INIT         ((size_t)0xcbf29ce484222325ULL)
#else
// 2^24 + 2^8 + 0x93 = 16777619
#   define FNV_PRIME        ((size_t)0x01000193)
#   define FNV_INIT         ((size_t)0x811c9dc5)
#endif

static size_t str2key (const char* str, size_t length)
{
    const unsigned char* s = (const unsigned char*)str;
    size_t hval = FNV_INIT;

    if (str == NULL)
        return 0;

    if (length == 0)
        length = strlen(str);

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*s && length) {

        /* xor the bottom with the current octet */
        hval ^= (size_t)*s++;
        length--;

        /* multiply by the FNV magic prime */
#ifdef __GNUC__
#   if SIZEOF_SIZE_T == 8
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);
#   else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#   endif
#else
        hval *= FNV_PRIME;
#endif
    }

    /* return our new hash value */
    return hval;
}

static int categories[] = {
    -1,
    -1,
    LC_CTYPE,
    LC_ADDRESS,
    -1,
    -1,
    LC_IDENTIFICATION,
    -1,
    -1,
    LC_NUMERIC,
    LC_TIME,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    LC_MONETARY,
    -1,
    -1,
    -1,
    LC_PAPER,
    LC_TELEPHONE,
    -1,
    -1,
    -1,
    -1,
    LC_COLLATE,
    -1,
    -1,
    LC_NAME,
    LC_MEASUREMENT,
    -1,
    LC_MESSAGE,
    -1,
    -1,
    -1,
};

// 运气好，对上面的关键词，当用 37 对哈希值取模时，刚好没有重复
// 注意：#define SIZEOF_SIZE_T 8 时成立。详情见：
// <https://gitlab.fmsoft.cn/VincentWei/best-practices-of-c/blob/master/src/samples/str2key.c>
int get_locale_category_by_keyword(const char *keyword)
{
    size_t hval = str2key(keyword) % (sizeof(categories)/sizeof(categories[0]));

    return categories[hval];
}
```

	
#### 屠龙版本：字符串原子化

1. 原子（Atom，也叫 Quark）表示一个可以唯一性确定一个字符串常量的整数值
1. 背后的数据结构是一个 AVL 树或者是红黑树，保存着字符串常量和整数之间的映射关系

	
1. 原先要分配缓冲区存储指针的地方，现在只需要存储一个整数
1. 原先调用 `strcmp` 对比字符串的地方，现在可使用 `==` 直接对比
1. 原先使用复杂判断的地方，现在可以使用 `switch` 语句
1. 通用，综合性能未必最佳

	
HVML 解释器 PurC 中的相关接口：

```c
typedef unsigned int purc_atom_t;

PCA_EXPORT purc_atom_t
purc_atom_from_string(const char* string);

PCA_EXPORT purc_atom_t
purc_atom_from_static_string(const char* string);

PCA_EXPORT purc_atom_t
purc_atom_try_string(const char* string);

PCA_EXPORT const char*
purc_atom_to_string(purc_atom_t atom);
```

	
#### 倚天版本：区分名字空间的字符串原子化

1. 按照不同的命名空间管理字符串常量
1. 避免不同命名空间中的相同关键词具有相同原子值

	
HVML 解释器 PurC 中的增强接口：

```c
#define PURC_ATOM_BUCKET_BITS   4
#define PURC_ATOM_BUCKETS_NR    (1 << PURC_ATOM_BUCKET_BITS)

PCA_EXPORT purc_atom_t
purc_atom_from_string_ex(int bucket, const char* string);

static inline purc_atom_t purc_atom_from_string(const char* string) {
    return purc_atom_from_string_ex(0, string);
}

PCA_EXPORT purc_atom_t
purc_atom_from_static_string_ex(int bucket, const char* string);

static inline purc_atom_t purc_atom_from_static_string(const char* string) {
    return purc_atom_from_static_string_ex(0, string);
}
```

		
## 数据结构决定性能

1. 线性访问，整数运算永远最快
1. 通常我们需要用空间换速度
1. 同样的问题，要根据目标数据量体裁衣，不能僵化

	
### 如何判断一个自然数是否是素数

1. 16 以内？
1. 1024 以内？
1. 任意 64 位自然数？
1. 任意自然数？

	
### 小于等于 16

```c
// 依次判断
bool is_prime_16_v0(unsigned int n)
{
    assert(n < 16);

    if (n == 2 || n == 3 || n == 5 || n == 7 ||
            n == 11 || n == 13)
        return true;
    else
        return false;
}

// 查表
bool is_prime_16_v1(unsigned int n)
{
    static bool prime_or_not[] = {
        false,      // 0
        false,      // 1
        true,      // 2
        true,      // 3
        false,      // 4
        true,      // 5
        false,      // 6
        true,      // 7
        false,      // 8
        false,      // 9
        false,      // 10
        true,      // 11
        false,      // 12
        true,      // 13
        false,      // 14
        false,      // 15
    };

    assert(n < 16);
    return prime_or_not_16[n];
}

// 最佳平衡：使用位表示是否素数
bool is_prime_16_v3(unsigned int n)
{
    static const unsigned short bits = 0x28AC; // 0010.1000.1010.1100

    assert(n < 16);
    return (bits >> n) & 0x01;
}
```

	
### 小于等于 1024

```c
// 二次索引查表
bool is_prime_1024_v1(unsigned int n)
{
    static unsigned short prime_bits[] = {
        0x28AC, // 0010.1000.1010.1100
        ...
    };

    assert(n < sizeof(prime_bits)/sizeof(unsigned short) * 16);
    return ((prime_bits[n / 16]) >> n) & 0x01;
}

// 二分查找
bool is_prime_1024_v2(unsigned int n)
{
    static unsigned int primes[] = {
        2, 3, 5, 7, 11, 13, ...
    };

    assert(n < sizeof(primes)/sizeof(primes[0]));

    ssize_t low, high, mid;

    low = 0;
    high = sizeof(primes)/sizeof(primes[0]) - 1;
    while (low <= high) {
        int cmp;

        mid = (low + high) / 2;
        if (n == primes[mid]) {
            return true;
        }
        else if (n < primes[mid]) {
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }

    return false;
}
```
	
### 任意 64 位自然数

- 小于一千万的素数：664,579
- 64 位无符号整数最大值：18,446,744,073,709,551,615

	
### 任意自然数

- 已知最大的素数：2^30402457-1，共有 9,152,052 个十进制位

		
## 下一讲预告

- 主题：单元测试和可维护性
- 地点：微信视频号“考鼎录”直播间
- 时间：2022 年 1 月 27 日（周四）17：30

		
## Q & A

