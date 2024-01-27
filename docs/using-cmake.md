## 第五章：善用构建系统生成器

1. 常用构建系统和构建系统生成器
1. 充分利用构建系统生成器
1. CMake 构建体系模板

		
## 常用构建系统和构建系统生成器

- 构建系统（build system）：Make，Ninja
- 构建系统生成器（build system generator）：GNU Autotool，CMake，Meson，Gn
  - 也叫元构建系统（meta build system）

	
构建系统的要点：

- 目标（targets）
- 依赖关系（dependencies）
- 规则（rules）

```makefile
hello:hello.c
	cc -Wall -o hello hello.c
```

	
构建系统生成器

- GNU Autotool：主要用于 UNIX 类系统，使用繁琐。
- CMake：跨平台。
- Meson：跨平台，但主要用于 Linux FreeDesktop 开源项目。

最简单的 `CMakeLists.txt` 文件：

```cmake
project(hello_world)
add_executable(hello_world hello.c)
```

最简单的 `meson.build` 文件：

```meson
project('hello_world', 'c')
executable('hello_world' 'hello.c')
```

		
## CMake 构建系统生成器

	
定义构建类型

- `Debug`：不定义 `NDEBUG` 宏，包含有调试信息，最低优化级别。
- `Release`：定义 `NDEBUG` 宏，不包含调试信息，默认优化级别。
- `RelWithDebInfo`：定义 `NDEBUG` 宏，包含调试信息，默认优化级别。
- `MinSizeRel`：定义 `NDEBUG` 宏，不包含调试信息，最小尺寸优化级别。

	
```c
#ifdef NDEBUG

#define PC_ASSERT(cond)                                 \
    do {                                                \
        if (0 && !(cond)) {                             \
            /* do nothing */                            \
        }                                               \
    } while (0)

#define PC_DEBUG(x, ...)                                \
    if (0)                                              \
        purc_log_debug(x, ##__VA_ARGS__)

#else /* defined NDEBUG */

#define PC_ASSERT(cond)                                                 \
    do {                                                                \
        if (!(cond)) {                                                  \
            purc_log_error("PurC assertion failure %s:%d: condition ‘"  \
                    #cond "’ failed\n",                                 \
                    __FILE__, __LINE__);                                \
            abort();                                                    \
        }                                                               \
    } while (0)

#define PC_DEBUG(x, ...)    purc_log_debug(x, ##__VA_ARGS__)

#endif /* not defined NDEBUG */
```

	
处理平台差异

- 确定编译器、链接器
- 基础函数库功能/特性的测试
   - 标准函数库的实现差异。
   - 指定版本的第三方函数库是否存在。

	
CMake 生成的功能测试宏

```c
#define HAVE_GLIB 1                 /* 检测到 Glib 函数库 */
#define HAVE_GLIB_LESS_2_70 0       /* 检测到 Glib 函数库的版本高于 2.70 */
#define HAVE_INT128_T 1             /* 检测到支持 int128_t 数据类型 */
#define HAVE_LIBGCRYPT 1            /* 检测到 libgcrypt 函数库 */
#define HAVE_LIBSOUP 1              /* 检测到 libsoup 函数库 */
#define HAVE_LIBXML2 1              /* 检测到 libxml2 函数库 */
#define HAVE_VASPRINTF 1            /* 检测到 vasprintf 函数 */
```

	
使用功能测试宏

```c
#if !HAVE(VASPRINTF)    // #if defined(VASPRINTF) && VASPRINTF
int vasprintf(char **buf, const char *fmt, va_list ap);
#endif
```

```c
#if HAVE(GLIB)
#include <gmodule.h>

/* 使用 Glib 提供的 g_slice_xxx() 接口实现变体的分配和释放 */
purc_variant *pcvariant_alloc(void) {
    return (purc_variant *)g_slice_alloc(sizeof(purc_variant));
}

purc_variant *pcvariant_alloc_0(void) {
    return (purc_variant *)g_slice_alloc0(sizeof(purc_variant));
}

void pcvariant_free(purc_variant *v) {
    return g_slice_free1(sizeof(purc_variant), (gpointer)v);
}

#else   /* HAVE(GLIB) */

/* 使用标准的 malloc/calloc/free 接口实现变体的分配和释放 */
purc_variant *pcvariant_alloc(void) {
    return (purc_variant *)malloc(sizeof(purc_variant));
}

purc_variant *pcvariant_alloc_0(void) {
    return (purc_variant *)calloc(1, sizeof(purc_variant));
}

void pcvariant_free(purc_variant *v) {
    return free(v);
}

#endif   /* !HAVE(GLIB) */
```

	
自动生成代码

CMake 变量定义

```cmake
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "9")
set(PROJECT_VERSION_MICRO "8")
set(PROJECT_VERSION "0.9.8")
```

模板文件

```c
#define PURC_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define PURC_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define PURC_VERSION_MICRO @PROJECT_VERSION_MICRO@
#define PURC_VERSION_STRING "@PROJECT_VERSION@"

static inline void
purc_get_versions (int *major, int *minor, int *micro) {
    if (major) *major = PURC_VERSION_MAJOR;
    if (minor) *minor = PURC_VERSION_MINOR;
    if (micro) *micro = PURC_VERSION_MICRO;
}

static inline const char *purc_get_version_string (void) {
    return PURC_VERSION_STRING;
}
```

		
## CMake 构建体系模板

- GitHub：<https://github.com/VincentWei/best-practices-of-c>
- GitLab：<https://gitlab.fmsoft.cn/VincentWei/best-practices-of-c>

或者：

- GitHub：<https://github.com/VincentWei/cmake-project-template>
- GitLab：<https://gitlab.fmsoft.cn/VincentWei/cmake-project-template>

		
## 实操

使用构建体系模板管理一个新项目

		
## 作业

1. 使用本章介绍的 CMake 构建体系模板管理 libubox 项目

