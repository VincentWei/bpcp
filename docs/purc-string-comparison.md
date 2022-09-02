## 字符串对比的N种实现

- HVML 规范：<https://github.com/HVML/html-docs>
- HVML 解释器：<https://github.com/HVML/PurC>

		
## 字符串匹配无处不在

- 各种关键词匹配
   - 标签名称
   - 属性名称
   - 特殊数据：`undefined`、`null`、`true`、`false`。
- 动态对象方法使用的各类关键词参数
- 通讯协议数据包中的关键词识别

	
```js
$DATETIME.time_prt(
        <'atom | cookie | iso8601 | rfc822 | rfc850 | rfc1036 | rfc1036 | rfc1123 | rfc7231 | rfc2822 | rfc3339 | rfc3339-ex | rss | w3c' $format = 'iso8601':
            - 'atom':       `Atom (example: 2005-08-15T15:52:01+00:00)`
            - 'cookie':     `HTTP Cookies (example: Monday, 15-Aug-2005 15:52:01 UTC)`
            - 'iso8601':    `Same as 'ATOM' (example: 2005-08-15T15:52:01+00:00)`
            - 'rfc822':     `RFC 822 (example: Mon, 15 Aug 05 15:52:01 +0000)`
            - 'rfc850':     `RFC 850 (example: Monday, 15-Aug-05 15:52:01 UTC)`
            - 'rfc1036':    `RFC 1036 (example: Mon, 15 Aug 05 15:52:01 +0000)`
            - 'rfc1123':    `RFC 1123 (example: Mon, 15 Aug 2005 15:52:01 +0000)`
            - 'rfc7231':    `RFC 7231 (since PHP 7.0.19 and 7.1.5) (example: Sat, 30 Apr 2016 17:52:13 GMT)`
            - 'rfc2822':    `RFC 2822 (example: Mon, 15 Aug 2005 15:52:01 +0000)`
            - 'rfc3339':    `Same as 'ATOM'`
            - 'rfc3339-ex': `RFC 3339 EXTENDED format (example: 2005-08-15T15:52:01.000+00:00)`
            - 'rss':        `RSS (example: Mon, 15 Aug 2005 15:52:01 +0000)`
            - 'w3c':        `World Wide Web Consortium (example: 2005-08-15T15:52:01+00:00)`
        >
        [, <null | number | longint | ulongint | longdouble $seconds: `seconds since Epoch; @null for current calendar time.`>
            [, <string $timezone>
            ]
        ]
) string | false: `a date and time string in the given time format $format and the time zone $timezone for the specified calendar time $seconds.`

$DATETIME.time_prt
    // string: '2020-06-24T11:27:05+08:00'

$DATETIME.time_prt.iso8601
    // string: '2020-06-24T11:27:05+08:00'

$DATETIME.time_prt('rfc3339-ex')
    // string: '2020-06-24T11:27:05.387+08:00'
```

		
## 问题的描述

```js
switch (os_name) {
    case "Linux":
        ...
        break;

    case "Windows":
        ...
        break;

    case "HybridOS":
        ...
        break;

    default:
        break;
}
```

		
## 最基础的实现

```c
    if (strcmp(os_name, "Linux") == 0) {
        ...
    }
    else if (strcmp(os_name, "Windows") == 0) {
        ...
    }
    else if (strcmp(os_name, "HybridOS") == 0) {
        ...
    }
```

		
## 优化方案一：首字母散列

```c
static purc_nvariant_method
property_getter(const char *name)
{
    switch (name[0]) {
    case 's':
        if (strcmp(name, "send") == 0) {
            return send_getter;
        }
        break;

    case 'r':
        if (strcmp(name, "recv") == 0) {
            return recv_getter;
        }
        break;

    case 'c':
        if (strcmp(name, "cap") == 0) {
            return cap_getter;
        }
        break;

    case 'l':
        if (strcmp(name, "len") == 0) {
            return len_getter;
        }
        break;

    default:
        break;
    }

    return NULL;
}
```

		
## 优化方案一的增强版：如果有强迫症

```c
static purc_nvariant_method
property_getter(const char *name)
{
    switch (name[0]) {
    case 's':
        if (strcmp(name + 1, "end") == 0) {
            return send_getter;
        }
        break;

    case 'r':
        if (strcmp(name + 1, "ecv") == 0) {
            return recv_getter;
        }
        break;

    case 'c':
        if (strcmp(name + 1, "ap") == 0) {
            return cap_getter;
        }
        break;

    case 'l':
        if (strcmp(name + 1, "en") == 0) {
            return len_getter;
        }
        break;

    default:
        break;
    }

    return NULL;
}
```

		
## 优化方案二：二分查找法

1. 线性的结构数组定义字符串以及对应的操作函数
1. 按字符串排好序

<https://github.com/HVML/PurC/blob/master/Source/PurC/pcrdr/message.c#L643>

		
## 优化方案三：原子字符串

1. 将字符串常量和唯一的整数值（原子值）建立一一映射关系
1. 匹配时对比字符串的原子值

<https://github.com/HVML/PurC/blob/master/Source/PurC/dvobjs/system.c#L975>

		
## 原子字符串的实现

1. 哈希算法：<https://github.com/GNOME/glib/blob/main/glib/gquark.c>
1. 红黑树：<https://github.com/HVML/PurC/blob/master/Source/PurC/utils/atomstring.c>
1. PurC 原子字符串的增强
   - 使用多个桶（bucket）管理不同用途的字符串常量
   - 可以移除特定的字符串
