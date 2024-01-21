## 第二章：用好写好头文件

1. 头文件分类
1. 滥用头文件的坏处
1. 用好头文件的两大原则
1. 头文件的分类管理

		
## 复习一下

- 头文件的作用：用于声明公用的函数、宏、数据类型或结构。
- 头文件里边通常包含：函数声明、内联函数的实现、宏、枚举常量、数据类型、结构体的定义以及 API 接口描述文档。
- 头文件通常不包含：非内联函数的具体实现。

	
头文件分类

- 标准 C 库的头文件，通常含有 std 前缀，如 `stdio.h`、`stdlib.h` 等。
- 操作系统或者 POSIX 标准相关的头文件，比如 `unistd.h`、`sys/uio.h` 等。
- 平台特有头文件，如 `windows.h`。
- 第三方库头文件，如 `zlib.h`。
- 自定义头文件。

		
## 常用 C99 头文件

- `stdio.h`：标准 IO 接口：`printf()`, `scanf()`
- `stdlib.h`：通用工具：`atoi()`, `malloc()`, `free()`
- `stdarg.h`：可变参数处理：`va_list`
- `stddef.h`：常用类型以及宏的定义，如 `size_t`、`NULL` 等
- `string.h`/`strings.h`：字符串操作函数，如 `strcat()`, `strcasecmp()` 等
- `ctype.h`：字符操作函数或宏，如 `toupper()`
- `time.h`：时间和时区操作函数，如 `time()`
- `math.h`：数学函数，如 `M_PI`, `sin()`, `cos()`

	
C99 新增头文件
- `stdint.h`：整数类型及各整数类型的极值
- `stdbool.h`：bool 类型、true、false 等

		
## 常用 POSIX 标准头文件

- `unistd.h`：类型定义及常见系统调动接口声明，如 `pid_t`、`fork()` 等
- `fcntl.h`：文件控制接口，如 `creat()`
- `dirent.h`：目录操作接口，如 `opendir()`
- `sys/*.h`：系统调用专用的数据类型、常量以及接口等

		
## 头文件机制的复杂性

```c
#include <stdio.h>
#include <math.h>

int main(void)
{
    printf("PI in the current system: %f.\n", M_PI);
    return 0;
}
```

	
产生大量冗余内容

- [hello-pi-cpp-full.c](https://gitlab.fmsoft.cn/VincentWei/best-practices-of-c/-/blob/master/book-list/samples/hello-pi-cpp-full.c)

	
头文件保卫宏

```c
/* Define ISO C stdio on top of C++ iostreams.
   Copyright (C) 1991-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library,
   which is licensed under LGPLv2.1+. */

/*
 *	ISO C99 Standard: 7.19 Input/output	<stdio.h>
 */

#ifndef _STDIO_H
#define _STDIO_H	1

#define __GLIBC_INTERNAL_STARTING_HEADER_IMPLEMENTATION
#include <bits/libc-header-start.h>

__BEGIN_DECLS

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#define __need___va_list
#include <stdarg.h>

#include <bits/types.h>
#include <bits/types/__fpos_t.h>
#include <bits/types/__fpos64_t.h>
#include <bits/types/__FILE.h>
#include <bits/types/FILE.h>
#include <bits/types/struct_FILE.h>

#ifdef __USE_GNU
# include <bits/types/cookie_io_functions_t.h>
#endif

#if defined __USE_XOPEN || defined __USE_XOPEN2K8
# ifdef __GNUC__
#  ifndef _VA_LIST_DEFINED
typedef __gnuc_va_list va_list;
#   define _VA_LIST_DEFINED
#  endif
# else
#  include <stdarg.h>
# endif
#endif

... /* 略去中间的内容 */

#include <bits/floatn.h>
#if defined __LDBL_COMPAT || __LDOUBLE_REDIRECTS_TO_FLOAT128_ABI == 1
# include <bits/stdio-ldbl.h>
#endif

__END_DECLS

#endif /* <stdio.h> included.  */
```

	
头文件搜索目录

```console
$ gcc -E -v -I/srv/devel/include not-existing.c
```

	
功能测试宏

- 我们可利用某些功能测试宏，通过避免在系统头文件中暴露非标准的定义，从而帮助我们编写可移植的应用程序。
- 利用其他一些功能测试宏，可以暴露一些默认情况下不暴露的非标准的定义。

```c
#define _GNU_SOURCE /* 在所有的 #include 之前定义 _GNU_SOURCE */

#include <stdlib.h>
#include <string.h>

#ifndef __USE_POSIX
/* 当前环境不支持 POSIX (IEEE Std 1003.1) 扩展接口，`strdup()` 未被暴露。 */
static char *strdup(const char *s)
{
    char *dup = malloc(strlen(s) + 1);
    strcpy(dup, s);
    return dup;
}
#endif /* !__USE_POSIX */

static void foo(const char *str)
{
#ifndef __USE_GNU
    /* 若未定义 __USE_GNU 宏，表明功能测试宏 _GNU_SOUCRE 未生效。*/
    char *my_str = strdup(str);
#else
    char *my_str = strdupa(str);
#endif

    /* do something using my_str */
    ...

#ifndef __USE_GNU
    free(my_str);
#endif
}
```

	
针对 C++ 的特别处理

```c
/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef __cplusplus
# define __BEGIN_DECLS	extern "C" {
# define __END_DECLS	}
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif
```

		
## 滥用头文件的坏处

- 拖慢编译速度。
- 破坏可移植性。
   - POSIX 标准头文件并不是所有平台上都可用。
   - 某些我们常用的接口并不是 C99 标准接口。
   - 在 RTOS 或物联网小系统中，某些 C99 接口可能残缺，如标准 IO 相关接口。
- 容易造成命名污染。

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

...

```

		
## 自定义头文件中的常见问题

1. 不使用或不当定义头文件保卫宏
1. 未正确处理和 C++ 程序混用的情形
1. 未处理可能的重复定义
1. 包含不该出现在头文件中的内容
1. 未妥善处理可能的可移植性问题

	
```c
#ifndef UTILS_H
#define UTILS_H

...

#endif /* not defined UTILS_H */
```

	
```c
#ifndef __FOO_BAR_UTILIS_H_
#define __FOO_BAR_UTILLS_H_

...

#endif
```

	
```c
#pragma once

...
```

	
```c
#ifndef __FOO_BAR_UTILIS_H_
#define __FOO_BAR_UTILLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include "foo.h"

...

#ifdef __cplusplus
}
#endif

#endif /* not defined __FOO_BAR_UTILIS_H_ */
```

	
```c
typedef int BOOL;
enum {
    FALSE,
    TRUE
};

static inline bool MAX(intmax_t x, intmax_t y) {
    return (a<b)?b:a;
}

static inline bool MIN(intmax_t x, intmax_t y) {
    return (a<b)?a:b;
}
```

	
```c
#ifndef BOOL
#   define BOOL     int
#   define TRUE     1
#   define FALSE    0
#endif

#ifndef MIN
#   define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#   define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
```

	
```c
#include <stdlib.h>
#include <string.h>

#define MIN_ARGC        1
static int check_argc(int argc);

#define MIN_LEN_ARG     5
static int check_args(const char *argv[]);
```

	
```c
extern int asprintf (char **__ptr, const char *__fmt, ...)
        __attribute__ ((__format__ (__printf__, 2, 3)))
        __attribute__ ((__warn_unused_result__));
```

		
## 用好、写好头文件的两大原则

1. 最少包含
   - 只包含必需的头文件
   - 只包含会被多个源文件共用的内容
2. 自立（self-contained）
   - 任意一个头文件放到源文件的第一行，都不会出现未定义或者未声明的编译错误。

	
头文件中的典型内容

1. 宏定义。
1. 枚举量的定义。
1. 结构体的声明或定义。
1. 自定义数据类型。
1. 函数的原型声明。
1. 内联（inline）函数的实现。
1. 以注释形式存在的接口描述文本。

		
## 一般项目的四类头文件

- 对外头文件（API）
- 全局内部头文件
- 模块内部头文件
- 构建系统生成的头文件

		
## 命名、路径和安装规范

- 头文件命名
- 目标安装路径
- 编译时的头文件搜索路径

		
## 实操

HVML 开源解释器 PurC 的头文件组织

	
```console
.
├── build                               # 构建用目录，cmakeconfig.h 所在位置
│   └── Source                          # 编译生成的目标文件所在目录
├── Documents
│   └── screenshots
└── Source
    ├── PurC                            # config.h 所在位置
    │   ├── include                     # 外部头文件所在位置（具有 `purc-` 前缀，如 purc-variant.h）
    │   │   └── private                 # 全局内部头文件所在位置，如 list.h
    │   ├── document                    # 模块源文件及模块内部头文件所在位置，下同
    │   ├── dom
    │   ├── dvobjs
    │   │   └── parsers
    │   ├── ejson
    │   ├── executors
    │   │   └── parsers
```

	

```c
#ifndef FOO_CONFIG_H
#define FOO_CONFIG_H

#if defined(BUILDING_WITH_CMAKE)
#   include "cmakeconfig.h"
#elif defined(BUILDING_WITH_AUTOTOOLS)
#   include "acconfig.h"
#endif

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef __cplusplus
#   define FOO_BEGIN_DECLS	extern "C" {
#   define FOO_END_DECLS	}
#else
#   define FOO_BEGIN_DECLS
#   define FOO_END_DECLS
#endif

#if defined(__GNUC__) || defined(CLANG)
#   define FOO_ATTRIBUTE_PRINTF(format_string, extra_args)              \
        __attribute__((__format__(printf, format_string, extra_args)))
#   define FOO_ATTRIBUTE_WUR \
        __attribute__ ((__warn_unused_result__))
#else
#   define FOO_ATTRIBUTE_PRINTF(format_string, extra_args)
#   define FOO_ATTRIBUTE_WUR
#endif

#endif /* not defined FOO_CONFIG_H */
```
	

```c
#include "config.h"

#include "purc-variant.h"               /* 外部头文件 */

#include "private/atom-buckets.h"       /* 全局内部头文件 */
#include "private/variant.h"            /* 全局内部头文件 */
#include "private/instance.h"           /* 全局内部头文件 */
#include "private/utils.h"              /* 全局内部头文件 */

#include "variant-internals.h"          /* 模块内部头文件 */

#include <stdlib.h>                     /* C99 头文件 */
#include <string.h>                     /* C99 头文件 */

#if OS(LINUX) || OS(UNIX)
    #include <dlfcn.h>                  /* POSIX 头文件 */
#endif

#if HAVE(GLIB)
    #include <gmodule.h>                /* 第三方函数库头文件 */
#endif
```

	

```console
.
├── build                               # 构建用目录，cmakeconfig.h 所在位置
│   ├── DerivedSources
│   │   ├── ForwardingHeaders
│   │   │   ├── purc                    # 构建系统复制的 PurC 外部头文件所在位置
│   │   │   └── wtf
│   └── Source                          # 编译生成的目标文件所在目录
├── Documents
│   └── screenshots
└── Source
    ├── Executables
    │   ├── purc                        # 全局头文件及 config.h 所在位置
    │   │   ├── region                  # 模块源文件及模块内头文件，下同
    │   │   ├── strutil
    │   │   ├── tty
    │   │   ├── unicode
    │   │   └── util
    │   └── purc-fetcher
```

	
```c
#include "config.h"
#include "foil.h"           /* 全局头文件 */
#include "rect/rect.h"      /* 模块头文件 */

#include <purc/purc.h>      /* PurC 函数库外部头文件 */

#include <stdlib.h>         /* C99 头文件 */
#include <stdbool.h>        /* C99 头文件 */
#include <assert.h>         /* C99 头文件 */

#include <unistd.h>         /* POSIX 头文件 */
```

		
## 作业

1. 重构 OpenWRT [libubox](https://openwrt.org/docs/techref/libubox) 函数库的头文件。

