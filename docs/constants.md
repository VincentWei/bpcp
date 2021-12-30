## 第四讲：常量的定义和使用

- 时间：10 月 16 日 20:00 ~ 21:00
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 为什么要重视常量的管理？
1. 如何正确地定义和使用常量。
1. 如何优雅地定义和使用常量。
1. 使用宏生成常量和代码。
1. 原子字符串。

		
## 术语扫盲

- 常量（`constant`）：一般指数值，如 0, 0xFF, 3.14 等。
- 字符串字面值（`string literal`）：我们习惯了的字符串常量。

		
## 看看烂代码

	
不区分类型使用常量

```c
#include <stdbool.h>

static inline bool istab(char ch)
{
    return (ch == 0x09);
}
```

	
直接用常量定义数组等的大小

```c
    char buf[1024];

    sprintf(buf, "/home/%s/%s", user_name, file_name);

    ...

    char buf[16];
    sprintf(buf, "result: %lld", ll);
```

	
直接使用常量进行位运算

```c
static inline bool is_xxx(unsigned int flags)
{
    return flags & 0x0001;
}

static inline unsigned int set_xxx(unsigned int flags)
{
    return flags | 0x0001;
}

static inline bool is_yyy(unsigned int flags)
{
    return flags & 0x0002;
}

static inline unsigned int set_yyy(unsigned int flags)
{
    return flags | 0x0002;
}

static inline bool is_zzz(unsigned int flags)
{
    return flags & 0x0004;
}

static inline unsigned int set_zzz(unsigned int flags)
{
    return flags | 0x0004;
}
```

	
对重复出现的字符串常量，不使用宏定义

```c
    static struct pcdvojbs_dvobjs text[] = {
        {"head",     text_head_getter, NULL},
        {"tail",     text_tail_getter, NULL}
    };

    static struct pcdvojbs_dvobjs  bin[] = {
        {"head",     bin_head_getter, NULL},
        {"tai1",     bin_tail_getter, NULL}
    };
```

	
不善管理长字符串常量

```c
    n = snprintf (buff, sizeof(buff),
            "{\"packetType\":\"error\", \"protocolName\":\"%s\", \"protocolVersion\":%d, \"retCode\":%d, \"retMsg\":\"%s\" }",
            HIBUS_PROTOCOL_NAME, HIBUS_PROTOCOL_VERSION,
            err_code, hibus_get_ret_message (err_code));
```

	
在 `case` 语句中使用常量而不是枚举量

```c
static const char* get_method_name (int method)
{
    swith (method) {
    case 0:
        return "foo";
    case 1:
        return "bar";
    case 2:
        return "baz";
    case 3:
        return "qux";
    default:
        return "foobar";
    }
}

```

		
## 复习：立即数的数据类型

	
字符

```c
    char ch;
    unsigned char uch;

    ch = '\xFF';    // 十六进制
    ch = '\024';    // 八进制，一到三个小于 8 的数字组成，很少用到

    uch = '\xFF';   // 十六进制
    uch = '\124';   // 八进制，一到三个小于 8 的数字组成，很少用到
```

	
整数

```c
    int i = 100;
    unsigned int u;

    u = 100;                        // 十进制
    u = 0124;                       // 0 作为前缀，八进制
    u = 0x80000000U;                // 0x 作为前缀，十六进制
    u = (unsigned int)-1;           // 0xFFFFFFFF
    u = (unsigned int)-2;           // 0xFFFFFFFE

    long l = 0x80000000L;           // 后缀：l/L
    unsigned ul = 0x80000000UL;     // 后缀：U、L，顺序和大小写无关

    long long ll = 0x8000000000000000LL;
    unsinged long long ull = 0x8000000000000000ULL;
```

	
浮点数

```c
    float f1 = 0.1F;
    double f2 = 0.1;                // 没有后缀！
    long double f3 = 0.1L;
```

		
## 分析代码会在哪儿出问题

```c
#if defined(__LP64__)
#   define DEF_NR_TIMERS   32
#else
#   define DEF_NR_TIMERS   64
#endif

static int first_slot;
static unsigned long expired_timer_mask;

    ...

    int slot = first_slot;
    do {
        if (expired_timer_mask & (0x01 << slot))
            break;

        slot++;
        slot %= DEF_NR_TIMERS;
        if (slot == first_timer_slot) {
            slot = -1;
            break;
        }
    } while (1);

    first_timer_slot++;
    first_timer_slot %= DEF_NR_TIMERS;

    ...
```

		
## 复习：字符串常值的定义

```c
#include <assert.h>

#define EXT  ".bar"
#define NAME "foo"

static const char* name = "foo";
static const char* filename = "/home/" "user/" NAME EXT;

    ...

    /* Which assertion will fail?
    assert(sizeof(EXT) == 4);
    assert(sizeof(name) == 4);
    assert(sizeof(NAME) == 4);
```

		
## 正确定义和使用常量

	
使用正确的常量类型

```c
#include <stdbool.h>

static inline bool istab(char ch)
{
    return (ch == '\t');
}
```

	
使用正确的常量类型

```c
#if defined(__LP64__)
#   define DEF_NR_TIMERS   32
#else
#   define DEF_NR_TIMERS   64
#endif

static int first_slot;
static unsigned long expired_timer_mask;

    ...

    int slot = first_slot;
    do {
        /* use UL suffix!!! */
        if (expired_timer_mask & (0x01UL << slot))
            break;

        slot++;
        slot %= DEF_NR_TIMERS;
        if (slot == first_timer_slot) {
            slot = -1;
            break;
        }
    } while (1);

    first_timer_slot++;
    first_timer_slot %= DEF_NR_TIMERS;

    ...
```

	
使用宏定义数组大小

```c
#include <stdio.h>
#include <stdlib.h> // for PATH_MAX and NAME_MAX

#define FILE_EXT_TXT    ".txt"

    ...

    char buf[PATH_MAX + NAME_MAX + sizeof(FILE_EXT_TXT)];
    int n;

    /* use snprint instead of sprintf and check result */
    n = snprintf(buf, sizeof(buf), "/home/%s/%s" FILE_EXT_TXT,
            user_name, file_name);
    if (n < 0) {
        // snprintf error
        assert(0);
    }

    if ((size_t)n >= sizeof(buf)) {
        // truncated
        ...
    }
```

	
使用宏定义参与位运算的标志常量

```c
#define FLAG_XXX        0x0001
#define FLAG_YYY        0x0002
#define FLAG_ZZZ        0x0004

static inline bool is_xxx(unsigned int flags)
{
    return flags & FLAG_XXX;
}

static inline unsigned int set_xxx(unsigned int flags)
{
    return flags | FLAG_XXX;
}

static inline bool is_yyy(unsigned int flags)
{
    return flags & FLAG_YYY;
}

static inline unsigned int set_yyy(unsigned int flags)
{
    return flags | FLAG_YYY;
}

static inline bool is_zzz(unsigned int flags)
{
    return flags & FLAG_ZZZ;
}

static inline unsigned int set_zzz(unsigned int flags)
{
    return flags | FLAG_ZZZ;
}
```

	
对重复出现的字符串常量，使用宏定义

```c
#define DVOBJ_METHOD_FILE_HEAD  "head"
#define DVOBJ_METHOD_FILE_TAIL  "tail"

    static struct pcdvojbs_dvobjs text[] = {
        { DVOBJ_METHOD_FILE_HEAD, text_head_getter, NULL },
        { DVOBJ_METHOD_FILE_TAIL, text_tail_getter, NULL}
    };

    static struct pcdvojbs_dvobjs  bin[] = {
        { DVOBJ_METHOD_FILE_HEAD, bin_head_getter, NULL},
        { DVOBJ_METHOD_FILE_TAIL, bin_tail_getter, NULL}
    };
```

	
正确管理长字符串常量

```c
    n = snprintf(buff, sizeof (buff),
            "{"
            "\"packetType\":\"error\","
            "\"protocolName\":\"%s\","
            "\"protocolVersion\":%d,"
            "\"retCode\":%d,"
            "\"retMsg\":\"%s\""
            "}",
            HIBUS_PROTOCOL_NAME, HIBUS_PROTOCOL_VERSION,
            err_code, hibus_get_ret_message (err_code));
```

	
在 `case` 语句中使用枚举量

```c
enum method {
    IDM_FOO = 0,
    IDM_BAR,
    IDM_BAZ,
    IDM_QUX,
    IDM_FOOBAR,
};

static const char* get_method_name(enum method method)
{
    swith (method) {
    case IDM_FOO:
        return "foo";
    case IDM_BAR:
        return "bar";
    case IDM_BAZ:
        return "baz";
    case IDM_QUX:
        return "qux";
    case IDM_FOOBAR:
        return "foobar";
    }
}
```

		
## 优雅地定义和使用常量

	
让编译器帮我们计算常量

```c
#include <stdio.h>
#include <stdlib.h> // for PATH_MAX and NAME_MAX

#define FILE_EXT_TXT    ".txt"

    ...

    char buf[PATH_MAX + NAME_MAX + sizeof(FILE_EXT_TXT)];
```

	
让编译器帮我们计算常量

```c
#include <stdio.h>

#define ERR_UNKNOWN         1000
#define ERR_FOO             1999

#define ERR_MAX             ERR_FOO

#define _STRINGIFY(x)       #x
#define STRINGIFY(x)        _STRINGIFY(x)

#define ERR_CODE_MAX    STRINGIFY(ERR_MAX)

...

    char buf[sizeof(ERR_CODE_MAX)];
    sprintf(buf, "%d", ERR_FOO);
```

	
让编译器帮我们定义常量值

1. 定义 `xxx_FIRST` 和/或 `xxx_LAST` 枚举量
1. 定义 `xxx_MIN`、`xxx_MAX` 和 `xxx_NR` 宏

```c
typedef enum purc_variant_type
{
    PURC_VARIANT_TYPE_FIRST = 0,

    /* critical: keep order as is */
    PURC_VARIANT_TYPE_UNDEFINED = PURC_VARIANT_TYPE_FIRST,
    PURC_VARIANT_TYPE_NULL,
    PURC_VARIANT_TYPE_BOOLEAN,
    PURC_VARIANT_TYPE_NUMBER,
    PURC_VARIANT_TYPE_LONGINT,
    PURC_VARIANT_TYPE_ULONGINT,
    PURC_VARIANT_TYPE_LONGDOUBLE,
    PURC_VARIANT_TYPE_ATOMSTRING,
    PURC_VARIANT_TYPE_STRING,
    PURC_VARIANT_TYPE_BSEQUENCE,
    PURC_VARIANT_TYPE_DYNAMIC,
    PURC_VARIANT_TYPE_NATIVE,
    PURC_VARIANT_TYPE_OBJECT,
    PURC_VARIANT_TYPE_ARRAY,
    PURC_VARIANT_TYPE_SET,

    PURC_VARIANT_TYPE_LAST = PURC_VARIANT_TYPE_SET,
} purc_variant_type;

#define PURC_VARIANT_TYPE_MIN   PURC_VARIANT_TYPE_FIRST
#define PURC_VARIANT_TYPE_MAX   PURC_VARIANT_TYPE_LAST
#define PURC_VARIANT_TYPE_NR    (PURC_VARIANT_TYPE_MAX - PURC_VARIANT_TYPE_MIN + 1)

struct purc_variant_stat {
    size_t nr_values[PURC_VARIANT_TYPE_NR];
    size_t sz_mem[PURC_VARIANT_TYPE_NR];
    size_t nr_total_values;
    size_t sz_total_mem;
    size_t nr_reserved;
    size_t nr_max_reserved;
};
```

	
混用标志位和标识符

```c
#define WS_EX_WINTYPE_MASK          0x0000000FL

#define WS_EX_WINTYPE_TOOLTIP       0x00000001L
#define WS_EX_WINTYPE_GLOBAL        0x00000002L
#define WS_EX_WINTYPE_SCREENLOCK    0x00000003L
#define WS_EX_WINTYPE_DOCKER        0x00000004L
#define WS_EX_WINTYPE_NORMAL        0x00000005L

#define WS_EX_TROUNDCNS             0x00000010L
#define WS_EX_BROUNDCNS             0x00000020L

...

CreateMainWindow(...,
        WS_EX_TROUNDCNS | WS_EX_WINTYPE_NORMAL,
        ...);
...

switch(window_style & WS_EX_WINTYPE_MASK) {
case WS_EX_WINTYPE_TOOLTIP:
    ...
    break;
}

```

		
## 使用宏生成常量和代码

	
（1）

```c
enum method_id {
    DVOBJ_METHOD_FIRST = 0,

    DVOBJ_METHOD_head = DVOBJ_METHOD_FIRST,

    ...

    DVOBJ_METHOD_tail,

    DVOBJ_METHOD_LAST = DVOBJ_METHOD_tail,
};

#define METHOD_ID(name)     DVOBJ_METHOD_##name
#define METHOD_STR(name)    #name

static struct method_id_2_name {
    int         id;
    const char* name;
} _map [] = {
    { METHOD_ID(head), METHOD_STR(head) },
    { METHOD_ID(tail), METHOD_STR(tail) },
};

#define TABLE_SIZE(x)   (sizeof(x)/sizeof(x[0]))

...

    for (int i = 0; i < TABLE_SIZE(_map); i++) {
        printf("id: %d, name: %s\n",
                _map[i].id, _map[i].name);
    }
```

	
（2）

```c
#define FLAG_xxx        0x0001
#define FLAG_yyy        0x0002
#define FLAG_zzz        0x0004

#define IS_ENABLED(name) \
static inline bool is_##name##_enabled(unsigned int flags) \
{ \
    return flags & FLAG_##name; \
}

#define ENABLE(name) \
static inline unsigned int enable_##name(unsigned int flags) \
{ \
    return flags | FLAG_##name; \
}

IS_ENABLED(xxx)
ENABLE(xxx)

IS_ENABLED(yyy)
ENABLE(yyy)

IS_ENABLED(zzz)
ENABLE(zzz)
```
		
## 原子字符串

	
### 概念

- 原子字符串（AtomString，也叫 Quark）表示一个可以唯一性确定一个字符串常量的整数值
- 背后的数据结构是一个 AVL 树或者是红黑树，保存着字符串常量和整数之间的一一映射关系

	
### 好处

- 原先要分配缓冲区存储指针的地方，现在只需要存储一个整数
- 原先调用 `strcmp` 对比字符串的地方，现在可使用 `==` 直接对比
- 原先使用复杂判断的地方，现在可以使用 `switch` 语句

	
### 接口

HVML 解释器 PurC 中的相关接口：

```c
typedef uintptr_t purc_atom_t;

PCA_EXPORT purc_atom_t
purc_atom_from_string(const char* string);

PCA_EXPORT purc_atom_t
purc_atom_from_static_string(const char* string);

PCA_EXPORT purc_atom_t
purc_atom_try_string(const char* string);

PCA_EXPORT const char*
purc_atom_to_string(purc_atom_t atom);
```
	
### 不使用原子字符串

```c
struct hvml_element {
    char *tag;

    ...
}

    if (strcmp(element->tag, "iterate") == 0) {
        ...
    }
```

	
### 使用原子字符串

```c

struct hvml_element {
    purc_atom_t tag_atom;

    ...
}

enum hvml_tag_id {
    HVML_TAG_FIRST = 0,

    HVML_TAG_hvml = HVML_TAG_FIRST,
    HVML_TAG_head,
    HVML_TAG_body,
    ...

    HVML_TAG_bind,
    HVML_TAG_LAST = HVML_TAG_bind,
}

#define PURC_VARIANT_TYPE_MIN   PURC_VARIANT_TYPE_FIRST
#define PURC_VARIANT_TYPE_MAX   PURC_VARIANT_TYPE_LAST

#define HVML_TAG_MIN    HVML_TAG_FIRST
#define HVML_TAG_MAX    HVML_TAG_LAST
#define HVML_TAG_NR     (HVML_TAG_MAX - HVML_TAG_MIN + 1)

static struct hvml_tag_info {
    purc_atom_t     atom;
    const char*     name;

    unsigned int    flags;
    ...

} hvml_tag_info [] = {
    { 0, "hvml", HVML_TAG_TYPE_FRAMEWORK, ... },
    { 0, "head", HVML_TAG_TYPE_FRAMEWORK, ... },
    { 0, "body", HVML_TAG_TYPE_FRAMEWORK, ... },

    ...

    { 0, "bind", HVML_TAG_TYPE_ACTION, ... },

    { 0, "__unknown", HVML_TAG_TYPE_FOREIGN, ... },
};

    // initialize the atomstring
    assert(sizeof(hvml_tag_info) >= HVML_TAG_NR)

    for (int i = 0; i < HVML_TAG_NR; i ++) {
        hvml_tag_info[i].atom
            = purc_atom_from_static_string(hvml_tag_info[i].name);
    }

    ...

    if (element->atom_tag == hvml_tag_info[HVML_TAG_hvml].atom) {
        ...
    }
```

		
## Q & A

问题解答

