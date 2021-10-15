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

		
## 滥用常量的表现形式

		
## 看看烂代码

	
不区分类型使用常量

```c
#if defined(__LP64__)
#   define DEF_NR_TIMERS   32
#else
#   define DEF_NR_TIMERS   64
#endif

static int first_slot;
unsigned long expired_timer_mask;

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

	
直接用常量定义数组等数组的大小

```c
```

	
直接使用常量进行位运算

```c
```

	
对重复出现的字符串常量，不使用宏定义


	
在 `case` 语句中使用常量而不是枚举量

		
## 正确使用立即数的数据类型

1. 字符

```c
    char ch;
    unsigned char uch;

    ch = '\0';
    ch = '\xFF';

    uch = '\xFF';
    uch = '\456';
```

	
整数

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

	
浮点数

```c
    float f1 = 0.1F;
    double f2 = 0.1;
    long double f3 = 0.1L;
```

		
## 错误用法

		
## Q & A

