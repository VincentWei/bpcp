## 第四讲：常量的定义和使用

- 时间：10 月 16 日 20:00 ~ 21:00
- 地点：微信视频号“考鼎录”直播间
- [立即预约](#/3)

		
## 提纲

1. 为什么要重视常量的管理？
1. 如何优雅地定义和使用常量。
1. 枚举量的使用技巧。
1. 使用宏自动生成字符串常量。
1. 其他有关常量使用的技巧。

		
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

- 字符

```c
    char ch;
    unsigned char uch;

    ch = '\xFF';    // 十六进制
    ch = '\024';    // 八进制，一到三个小于 8 的数字组成，很少用到

    uch = '\xFF';   // 十六进制
    uch = '\124';   // 八进制，一到三个小于 8 的数字组成，很少用到
```

	
- 整数

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

	
- 浮点数

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

		
## 优雅地定义和使用常量

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

		
## Q & A

