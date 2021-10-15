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
		
## 难以发现的常量使用问题

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

		
## 复习：立即数的数据类型

### 字符

```c
    char ch;
    unsigned char uch;

    ch = '\0';
    ch = '\xFF';

    uch = '\xFF';
    uch = '\456';
```

	
### 整数

```c
    int i = 100;
    unsigned int u;

    u = 100;
    u = 0x80000000U;
    u = (unsigned int)-1;

    long l = 0x80000000L;
    unsigned ul = 0x80000000UL;

    long long ll = 0x8000000000000000LL;
    unsinged long long ull = 0x8000000000000000ULL;
```

	
### 浮点数

```c
    float f1 = 0.1F;
    double f2 = 0.1;
    long double f3 = 0.1L;
```

		
## 复习：字符串常值的定义

```c
#include <assert.h>

#define NAME "foo"

static const char* name = "foo";
static const char* filename = "/home/" "user/" ".bashrc";

    ...

    assert(sizeof(name) == 4);
    assert(sizeof(NAME) == 4);
```

		
## 优雅地定义和使用常量

		
## Q & A

