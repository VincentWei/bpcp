## 第十二讲：高效调试

- 时间：2022 年 1 月 29 日（周六）17：30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 高效调试的基本原则
1. C 程序常见错误分类
1. 内存使用错误
1. 善用调试工具

		
## 高效调试的基本原则

1. 多读代码、读懂代码
1. 不要慌，不要怀疑一切，尤其不要轻易怀疑编译工具链
1. 快速找到第一现场，理清现象和缺陷的内在关联
   - 复原现场，形成完整的证据链
   - 知其然，更要知其所以然
1. 避免使用低效调试手段

		
## C 程序常见错误分类

1. 编译、链接错误（5%）
1. 算法或逻辑错误（20%）
1. 内存使用错误（70%），其中一半会导致逻辑错误
1. 疑难杂症（5%）

	
### 编译、链接错误

1. 编译错误
   - 始终解决第一个错误
   - 正确理解编译器给出的错误描述
1. 链接错误：未找到某某符号
   - 正确的函数库（包括版本）和库搜索路径
   - 静态链接时，根据函数库依赖关系，调整函数库顺序

	
### 算法或逻辑错误

- 主要靠读代码和单元测试

	
### 内存使用错误

1. 静态数据使用错误
1. 堆使用错误
1. 栈使用错误

	
### 疑难杂症

1) 命名污染，导致调用错误的函数或者使用错误的全局变量
2) 整数溢出：逻辑错误（死循环）
```c
    unsigned char byte;
    for (byte = 0; byte <= 0xFF; byte++) {
        ...
    }
```
3) 异步数据访问
   - Unix 系统中的信号处理
   - 多线程程序中出现竞态（race-condition）


		
## 内存使用错误

	
### 静态数据使用错误

1. 越界访问
1. 最常见的影响：
    - 数据被意外篡改导致逻辑错误
    - 程序崩溃

	
### 静态数据的布局算法对错误现象的影响

1) 示例1

```c
static char buff1[8];
static char buff2[8];

static void access_out_of_range(unsigned int range)
{
    printf("Going to set %d bytes to `$` in buff1\n", range);
    memset(buff1, '$', range);

    puts("Content in buff1");
    for (int i = 0; i < 8; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 8; i++) {
        putchar(buff2[i]);
    }
    puts("");
}

int main(void)
{
    unsigned int range = 8;

    printf("buff1(%p), buff2(%p)\n", buff1, buff2);

    strcpy(buff2, "hello");
    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
buff1(0x55f3374df018), buff2(0x55f3374df020)
Going to set 8 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 16 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 32 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 64 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 128 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 256 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 512 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 1024 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 2048 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
$$$$$$$$
Going to set 4096 bytes to `$` in buff1
Segmentation fault (core dumped)
```

	
### 静态数据的布局算法对错误现象的影响

2) 示例2

```c
static char buff1[8];
static char buff2[8] = "hello";

static void access_out_of_range(unsigned int range)
{
    printf("Going to set %d bytes to `$` in buff1\n", range);
    memset(buff1, '$', range);

    puts("Content in buff1");
    for (int i = 0; i < 8; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 8; i++) {
        putchar(buff2[i]);
    }
    puts("");
}

int main(void)
{
    unsigned int range = 8;

    printf("buff1(%p), buff2(%p)\n", buff1, buff2);
    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
buff1(0x55b6ee358020), buff2(0x55b6ee358010)
Going to set 8 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 16 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 32 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 64 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 128 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 256 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 512 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 1024 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 2048 bytes to `$` in buff1
Content in buff1
$$$$$$$$
Content in buff2
hello
Going to set 4096 bytes to `$` in buff1
Segmentation fault (core dumped)
```

	
### 堆使用错误

1. 越界访问
1. 使用无效地址（空指针、野指针或者已释放的指针）
1. 内存泄露（忘记释放）
1. 两次释放
1. 释放非分配地址
1. 最常见的影响
    - 数据被意外篡改导致逻辑错误
    - 程序崩溃

	
### 堆管理算法对错误现象的影响

1) 示例1

```c
#define CSTR_HELLO  "hello, world!"

static void access_out_of_range(unsigned int range)
{
    const char *hello = CSTR_HELLO;
    char *buff;

    buff = malloc(sizeof(char) * 4);

    if (range > sizeof(CSTR_HELLO)) {
        printf("Going to memset %d bytes\n", range);
        memset(buff, 0, range);
    }
    else {
        for (int i = 0; i < range; i++) {
            buff[i] = hello[i];
        }

        for (int i = 0; i < range; i++) {
            putchar(buff[i]);
        }
        puts("");
    }

    free(buff);
}

int main(void)
{
    unsigned int range = 8;

    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
hello, w
Going to memset 16 bytes
Going to memset 32 bytes
Going to memset 64 bytes
Going to memset 128 bytes
Going to memset 256 bytes
Going to memset 512 bytes
Going to memset 1024 bytes
Going to memset 2048 bytes
Going to memset 4096 bytes
Going to memset 8192 bytes
Going to memset 16384 bytes
Going to memset 32768 bytes
Going to memset 65536 bytes
Going to memset 131072 bytes
Going to memset 262144 bytes
Segmentation fault (core dumped)
```

	
### 堆管理算法对错误现象的影响

2) 示例2

```c
static void access_out_of_range(unsigned int range)
{
    char *buff1, *buff2;

    buff1 = calloc(sizeof(char),  4);
    buff2 = calloc(sizeof(char),  4);

    printf("Going to set %d bytes to `$` in buff1 (%p)\n", range, buff1);
    memset(buff1, '$', range);

    printf("Going to set %d bytes to `^` in buff2 (%p)\n", range, buff2);
    memset(buff2, '^', range);

    puts("Content in buff1");
    for (int i = 0; i < 4; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 4; i++) {
        putchar(buff2[i]);
    }
    puts("");

    free(buff1);
    free(buff2);
}

int main(void)
{
    unsigned int range = 8;

    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
Going to set 8 bytes to `$` in buff1 (0x56425fc582a0)
Going to set 8 bytes to `^` in buff2 (0x56425fc582c0)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 16 bytes to `$` in buff1 (0x56425fc586f0)
Going to set 16 bytes to `^` in buff2 (0x56425fc58710)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 32 bytes to `$` in buff1 (0x56425fc58730)
Going to set 32 bytes to `^` in buff2 (0x56425fc58750)
Content in buff1
$$$$
Content in buff2
^^^^
Segmentation fault (core dumped)
```

	
### 堆管理算法对错误现象的影响

3) 示例3

```c
#include <sys/mman.h>

void *malloc(size_t size)
{
    return mmap(NULL, size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void free(void *ptr)
{
    munmap(ptr, 0);
}

void *realloc(void *ptr, size_t size)
{
    assert(0);
    return NULL;
}

void *calloc(size_t nelem, size_t elsize)
{
    size_t sz = nelem * elsize;
    void *p = malloc (sz);
    memset(p, 0, sz);
    return p;
}

static void access_out_of_range(unsigned int range)
{
    char *buff1, *buff2;

    buff1 = calloc(sizeof(char),  4);
    buff2 = calloc(sizeof(char),  4);

    printf("Going to set %d bytes to `$` in buff1 (%p)\n", range, buff1);
    memset(buff1, '$', range);

    printf("Going to set %d bytes to `^` in buff2 (%p)\n", range, buff2);
    memset(buff2, '^', range);

    puts("Content in buff1");
    for (int i = 0; i < 4; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 4; i++) {
        putchar(buff2[i]);
    }
    puts("");

    free(buff1);
    free(buff2);
}

int main(void)
{
    unsigned int range = 8;

    for (; range < UINT_MAX; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
Going to set 8 bytes to `$` in buff1 (0x7fb3234fe000)
Going to set 8 bytes to `^` in buff2 (0x7fb3234d1000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 16 bytes to `$` in buff1 (0x7fb3234cf000)
Going to set 16 bytes to `^` in buff2 (0x7fb3234ce000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 32 bytes to `$` in buff1 (0x7fb3234cd000)
Going to set 32 bytes to `^` in buff2 (0x7fb3234cc000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 64 bytes to `$` in buff1 (0x7fb3234cb000)
Going to set 64 bytes to `^` in buff2 (0x7fb3234ca000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 128 bytes to `$` in buff1 (0x7fb3234c9000)
Going to set 128 bytes to `^` in buff2 (0x7fb3234c8000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 256 bytes to `$` in buff1 (0x7fb3234c7000)
Going to set 256 bytes to `^` in buff2 (0x7fb3234c6000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 512 bytes to `$` in buff1 (0x7fb3234c5000)
Going to set 512 bytes to `^` in buff2 (0x7fb3234c4000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 1024 bytes to `$` in buff1 (0x7fb3234c3000)
Going to set 1024 bytes to `^` in buff2 (0x7fb3234c2000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 2048 bytes to `$` in buff1 (0x7fb3234c1000)
Going to set 2048 bytes to `^` in buff2 (0x7fb3234c0000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 4096 bytes to `$` in buff1 (0x7fb3234bf000)
Going to set 4096 bytes to `^` in buff2 (0x7fb3234be000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 8192 bytes to `$` in buff1 (0x7fb3234bd000)
Going to set 8192 bytes to `^` in buff2 (0x7fb3234bc000)
Content in buff1
^^^^
Content in buff2
^^^^
Going to set 16384 bytes to `$` in buff1 (0x7fb3234bb000)
Going to set 16384 bytes to `^` in buff2 (0x7fb3234ba000)
Content in buff1
^^^^
Content in buff2
^^^^
Going to set 32768 bytes to `$` in buff1 (0x7fb3234b9000)
Going to set 32768 bytes to `^` in buff2 (0x7fb3234b8000)
Content in buff1
^^^^
Content in buff2
^^^^
Going to set 65536 bytes to `$` in buff1 (0x7fb3234b7000)
Going to set 65536 bytes to `^` in buff2 (0x7fb3232c2000)
Segmentation fault (core dumped)
```

	
### 栈使用错误

1. 越界访问
2. 栈溢出
3. 最常见的影响：
    - 数据被意外篡改导致逻辑错误
    - 跑飞
    - 程序崩溃

	
### 栈使用错误的现象

```c
static void access_out_of_range(unsigned int range)
{
    char buff[4];

    memset(buff, '$', range);
    printf("After setting %d bytes to `$` in buff (%p)\n", range, buff);

    puts("Content in buff");
    for (int i = 0; i < 4; i++) {
        putchar(buff[i]);
    }
    puts("");
}

int main(void)
{
    unsigned int range = 4;

    for (; range < UINT_MAX; range *= 2) {
        printf("Going to call access_out_of_range(%u)\n", range);
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

// Output
Going to call access_out_of_range(4)
After setting 4 bytes to `$` in buff (0x7fff3d81c274)
Content in buff
$$$$
Going to call access_out_of_range(8)
After setting 8 bytes to `$` in buff (0x7fff3d81c274)
Content in buff
$$$$
*** stack smashing detected ***: terminated
Aborted (core dumped)
```

	
### 调用可变参数函数的副作用

1. 可变参数（如格式化输入输出函数）在特定条件下会消除栈使用错误的影响

		
## 常用调试工具和技巧

1. 调试器：程序挂在哪儿了？
1. efence：程序是否有堆使用错误？
1. valgrind：是否存在内存泄露？
1. 打桩

	
### 调试器

- 调试器最大的价值在于快速找到程序挂在哪儿了。
- 单步跟踪代码的执行，效率极低，不建议使用。

	
### efence

```
$ gcc -Wall malloc-2.c -lefence
$ ./a.out
  Electric Fence 2.2 Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
Going to set 8 bytes to `$` in buff1 (0x7efd596c3ffc)
Segmentation fault (core dumped)

$ EF_PROTECT_BELOW=1 ./a.out 

  Electric Fence 2.2 Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
Going to set 8 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 8 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 16 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 16 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 32 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 32 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 64 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 64 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 128 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 128 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 256 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 256 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 512 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 512 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 1024 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 1024 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 2048 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 2048 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 4096 bytes to `$` in buff1 (0x7f761ff32000)
Going to set 4096 bytes to `^` in buff2 (0x7f761ff34000)
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 8192 bytes to `$` in buff1 (0x7f761ff32000)
Segmentation fault (core dumped)
```

	
### valgrind

```c
static void access_out_of_range(unsigned int range)
{
    char *buff1, *buff2;

    buff1 = calloc(sizeof(char),  4);
    buff2 = calloc(sizeof(char),  4);

    printf("Going to set %d bytes to `$` in buff1 (%p)\n", range, buff1);
    memset(buff1, '$', range);

    printf("Going to set %d bytes to `^` in buff2 (%p)\n", range, buff2);
    memset(buff2, '^', range);

    puts("Content in buff1");
    for (int i = 0; i < 4; i++) {
        putchar(buff1[i]);
    }
    puts("");

    puts("Content in buff2");
    for (int i = 0; i < 4; i++) {
        putchar(buff2[i]);
    }
    puts("");
}

int main(void)
{
    unsigned int range = 8;

    for (; range < 100; range *= 2) {
        access_out_of_range(range);
    }

    return EXIT_SUCCESS;
}

$ valgrind ./a.out
==8254== Memcheck, a memory error detector
==8254== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==8254== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==8254== Command: ./a.out
==8254==
Going to set 8 bytes to `$` in buff1 (0x4a5a040)
==8254== Invalid write of size 8
==8254==    at 0x4842994: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a040 is 0 bytes inside a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Going to set 8 bytes to `^` in buff2 (0x4a5a090)
==8254== Invalid write of size 8
==8254==    at 0x4842994: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a090 is 0 bytes inside a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 16 bytes to `$` in buff1 (0x4a5a520)
==8254== Invalid write of size 8
==8254==    at 0x48429B0: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a528 is 4 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Going to set 16 bytes to `^` in buff2 (0x4a5a570)
==8254== Invalid write of size 8
==8254==    at 0x48429B0: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a578 is 4 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 32 bytes to `$` in buff1 (0x4a5a5c0)
==8254== Invalid write of size 8
==8254==    at 0x4842964: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a5c0 is 0 bytes inside a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x4842967: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a5c8 is 4 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x484296B: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a5d0 is 12 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x484296F: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x10922B: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a5d8 is 20 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091E6: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Going to set 32 bytes to `^` in buff2 (0x4a5a610)
==8254== Invalid write of size 8
==8254==    at 0x4842964: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a610 is 0 bytes inside a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x4842967: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a618 is 4 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x484296B: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a620 is 12 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
==8254== Invalid write of size 8
==8254==    at 0x484296F: memset (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x109259: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==  Address 0x4a5a628 is 20 bytes after a block of size 4 alloc'd
==8254==    at 0x483DD99: calloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8254==    by 0x1091F9: access_out_of_range (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==    by 0x109305: main (in /srv/devel/courses/best-practices-of-c/src/samples/a.out)
==8254==
Content in buff1
$$$$
Content in buff2
^^^^
Going to set 64 bytes to `$` in buff1 (0x4a5a660)
Going to set 64 bytes to `^` in buff2 (0x4a5a6b0)
Content in buff1
$$$$
Content in buff2
^^^^
==8254==
==8254== HEAP SUMMARY:
==8254==     in use at exit: 32 bytes in 8 blocks
==8254==   total heap usage: 9 allocs, 1 frees, 1,056 bytes allocated
==8254==
==8254== LEAK SUMMARY:
==8254==    definitely lost: 32 bytes in 8 blocks
==8254==    indirectly lost: 0 bytes in 0 blocks
==8254==      possibly lost: 0 bytes in 0 blocks
==8254==    still reachable: 0 bytes in 0 blocks
==8254==         suppressed: 0 bytes in 0 blocks
==8254== Rerun with --leak-check=full to see details of leaked memory
==8254==
==8254== For lists of detected and suppressed errors, rerun with: -s
==8254== ERROR SUMMARY: 30 errors from 12 contexts (suppressed: 0 from 0)
```

	
### 打桩

- 只在最关键的地方输出信息，并输出尽量全面的调试信息。
- 使用自定义的调试信息输出接口，比如向 `syslog` 输出或者指定的日志文件输出。

```c
#ifndef __STRING
#define __STRING(x) #x
#endif

#ifdef NDEBUG

#define PC_ASSERT(cond)                                 \
    do {                                                \
        if (!(cond)) {                                  \
            pcutils_error("PurC assert failed.\n");     \
            abort();                                    \
        }                                               \
    } while (0)

#else /* define NDEBUG */

#define PC_ASSERT(cond)                                                                         \
    do {                                                                                        \
        if (!(cond)) {                                                                          \
            pcutils_error("PurC assert failure %s:%d: condition \"" __STRING(cond) " failed\n", \
                     __FILE__, __LINE__);                                                       \
            assert(0);                                                                          \
        }                                                                                       \
    } while (0)

#endif /* not defined NDEBUG */

#define PC_ERROR(x, ...) pcutils_error(x, ##__VA_ARGS__)

#ifndef NDEBUG

# define PC_ENABLE_DEBUG(x) pcutils_enable_debug(x)
# define PC_ENABLE_SYSLOG(x) pcutils_enable_syslog(x)
# define PC_DEBUG(x, ...) pcutils_debug(x, ##__VA_ARGS__)
# define PC_INFO(x, ...) pcutils_info(x, ##__VA_ARGS__)

#else /* not defined NDEBUG */

# define PC_ENABLE_DEBUG(x)             \
    if (0)                              \
        pcutils_enable_debug(x)

# define PC_ENABLE_SYSLOG(x)            \
    if (0)                              \
        pcutils_set_syslog(x)

#define PC_DEBUG(x, ...)                \
    if (0)                              \
        pcutils_debug(x, ##__VA_ARGS__)

#define PC_INFO(x, ...)                 \
    if (0)                              \
        pcutils_info(x, ##__VA_ARGS__)

#endif /* defined NDEBUG */

#if HAVE(SYSLOG_H)
#include <syslog.h>
#endif /* HAVE_SYSLOG_H */

#include "private/debug.h"

static bool _syslog = false;
#ifdef NDEBUG
static bool _debug = false;
#else
static bool _debug = true;
#endif

void pcutils_enable_debug(bool debug)
{
    _debug = debug;
}

void pcutils_enable_syslog(bool syslog)
{
    _syslog = syslog;
}

void pcutils_debug(const char *msg, ...)
{
    if (_debug) {
        va_list ap;

        va_start(ap, msg);
#if HAVE(VSYSLOG)
        if (_syslog) {
            vsyslog(LOG_DEBUG, msg, ap);
        }
        else
#endif
            vprintf(msg, ap);
        va_end(ap);
    }
}

void pcutils_error(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
#if HAVE(VSYSLOG)
    if (_syslog) {
        vsyslog(LOG_ERR, msg, ap);
    }
    else
#endif
        vfprintf(stderr, msg, ap);
    va_end(ap);
}

void pcutils_info(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
#if HAVE(VSYSLOG)
    if (_syslog) {
        vsyslog(LOG_INFO, msg, ap);
    }
    else
#endif
        vfprintf(stderr, msg, ap);
    va_end(ap);
}
```

		
## Q & A

